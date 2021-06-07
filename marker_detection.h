#include "/home/icey/software/aruco-3.1.12/src/aruco.h"
#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <Eigen/Core>
/*
int64_t GetTime(void)
{
    struct timespec rt;
    clock_gettime(CLOCK_MONOTONIC, &rt);

    int64_t t;
    t = (int64_t)(rt.tv_sec) * 1000000 + rt.tv_nsec / 1000;

    return t;
}
*/

class groundtruth
{

public: 

    void intrinsic ( const std::string& setting_dir ) {
        cv::FileStorage fs ( setting_dir, cv::FileStorage::READ );
        if ( fs.isOpened() == false )
        {
            std::cout << "setting_dir cannot open!";
        }
    
        fx = fs["camera.fx"];
        fy = fs["camera.fy"];
        cx = fs["camera.cx"];
        cy = fs["camera.cy"];

        k1 = fs["camera.k1"];
        k2 = fs["camera.k2"];
        p1 = fs["camera.p1"];
        p2 = fs["camera.p2"];
        k3 = fs["camera.k3"];

        K = ( cv::Mat_<float> ( 3, 3 ) << fx, 0.0, cx, 0.0, fy, cy, 0.0, 0.0, 1.0);
        dist = ( cv::Mat_<float> ( 1, 5 ) << k1, k2, p1, p2, k3 );
	eK << fx, 0.0, cx, 0.0, fy, cy, 0.0, 0.0, 1.0;

    }

    bool undistort ( cv::Mat& image ) {

        if ( image.empty() == true )
        {
            std::cout << "image empty: " << image << std::endl;
            return false;
        }
        aruco::MarkerDetector MDetector;
        markers = MDetector.detect(image);
        for (size_t i = 0; i < markers.size(); i++){
            std::vector<cv::Point2f> undist_corner(4);
            std::vector<cv::Point2f> undistort_corner(4);
            std::vector<cv::Point2f> final_corner;
            std::vector<cv::Point2f> corner;
            for (int j = 0; j < 4; j++){
                //std::cout << "marker: " << markers[i][j] << std::endl;
                corner.push_back(markers[i][j]);
                cv::undistortPoints ( corner, undistort_corner, K, dist, cv::noArray(), cv::noArray());
                undist_corner[j].x = undistort_corner[j].x * fx + cx;
                //std::cout << "undist_corner: " << undist_corner[j].x << std::endl;
                undist_corner[j].y = undistort_corner[j].y * fy + cy;
		        // final_corner.push_back( final_corner[j] );
                std::cout << "undistort corner: " << undist_corner << std::endl;
            }
            center_position =  (undist_corner[0] + undist_corner[1] + undist_corner[2] + undist_corner[3]) / 4;   
	        position = {center_position.x, center_position.y, (float)1.0};
	        //std::cout << "center_position: " << center_position.x << ", " << center_position.y << std::endl;
            markers[i].draw(image);
        }
    }
    
    void pnp ( const std::string& h_matrix_dir ) {
        cv::FileStorage fs_homography ( h_matrix_dir, cv::FileStorage::READ );
        cv::Mat cvH;
        fs_homography["homograph_matrix"] >> cvH;
        if ( fs_homography.isOpened() == false )
        {
            std::cout << "h_matrix_dir cannot open!";
        }
        //std::cout << "cvH: " << cvH << std::endl;
        H << cvH.at<double> ( 0, 0 ), cvH.at<double> ( 0, 1 ), cvH.at<double> ( 0, 2 ),
            cvH.at<double> ( 1, 0 ), cvH.at<double> ( 1, 1 ), cvH.at<double> ( 1, 2 ),
            cvH.at<double> ( 2, 0 ), cvH.at<double> ( 2, 1 ), cvH.at<double> ( 2, 2 );
        pt = H * position;
        pt = pt / pt[2];
        point = ( cv::Mat_<float> ( 1, 3 ) << pt[0], pt[1], pt[2]);
        std::cout << "pt: " << pt << std::endl;
        std::cout << "point: " << point << std::endl;
        //return point;
    }
    
    float fx;
    float fy;
    float cx;
    float cy;
    
    float k1;
    float k2;
    float p1;
    float p2;
    float k3;

    //float x;
    //float y;
    
    cv::Mat K;
    cv::Mat dist;
    Eigen::Matrix3d eK;
    cv::Mat point;

    Eigen::Matrix<float, 3, 3> H;
    cv::Mat cvH;
    cv::Point2f center_position;
    Eigen::Vector3f position;
    Eigen::Vector3f pt;
    //std::vector<Eigen::Vector3f> pt;
    std::vector<aruco::ARUCO_EXPORT Marker> markers;
    //std::vector<cv::Point2f> four_pixel;
    std::vector<cv::Point2f> four_pixel = std::vector<cv::Point2f>(4);
    std::vector<cv::Point2f> undist = std::vector<cv::Point2f>(4);
};  // class groundtruth

extern groundtruth gt;


