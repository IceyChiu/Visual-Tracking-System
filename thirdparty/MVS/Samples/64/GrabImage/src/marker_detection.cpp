/******************************
  Copyright (C) 2021 Icey Chiu
******************************/
#include <boost/lexical_cast.hpp>
#include "../include/marker_detection.h"

int main(int argc,char **argv){
//   if (argc != 2 ){ std::cerr<<"Usage: inimage"<<std::endl;return -1;}
     /* load parameters */
    const std::string h_matrix_dir = "/home/zebra/test/groundtruth/extrinsic_calibrationfile.yaml";
    std::string result_dir;
    const std::string setting_dir = "/home/zebra/test/groundtruth/intrinsic_calibrationfile.yaml";
    
    groundtruth gt;
    gt.intrinsic(setting_dir);
    aruco::MarkerDetector MDetector;
    MDetector.setDictionary("ARUCO");
    cv::String folder = "/home/zebra/Downloads/2020325/image/*.png";                                         
    std::vector<cv::String> filenames;
    std::vector<cv::String> names;
    std::vector<double> name;
    cv::glob(folder, filenames, false);
    int nImages = filenames.size();
    cv::Mat image;
    float start_x;
    float start_y;
    /*
    for (int i = 0; i < nImages; ++i)
    {
        images.push_back(cv::imread(filenames[i]));
    }
    */
    for (int i = 0; i < nImages; ++i)
    {
        image = cv::imread(filenames[i]);
        if (image.empty())
        {
            std::cout << "frame: " << image << std::endl;
            break;
        }
        
        gt.undistort(image);
        gt.pnp(h_matrix_dir, i);
        gt.markers;
        for (size_t i = 0; i < gt.markers.size(); i++){ 
            int id = gt.markers[i].id;
        
            switch ( id )
            {
                case 0:
                    result_dir = "/home/zebra/test/groundtruth/result_0.txt";
                    break;
                case 1:
                    result_dir = "/home/zebra/test/groundtruth/result_1.txt";
                    break;
                case 10:
                    result_dir = "/home/zebra/test/groundtruth/result_10.txt";
                    break;
                case 16:
                    result_dir = "/home/zebra/test/groundtruth/result_16.txt";
                    break;
                case 23:
                    result_dir = "/home/zebra/test/groundtruth/result_23.txt";
                    break;
            }
        }
        if (i == 0)
        {
            start_x = gt.pt[i][0];
            start_y = gt.pt[i][1];
        }
        std::ofstream  groundtruth_file(result_dir, std::ofstream::app);
	std::cout << filenames[i] << std::endl;
        names.push_back(filenames[i].substr(36, 19));
        name.push_back(std::stod(names[i], NULL) / 1000000000.00);
        if (i > 0)
            if (((name[i] - name[i - 1]) < 0.06) && ((abs(gt.pt[i][0] - gt.pt[i - 1][0]) > 0.02) || (abs(gt.pt[i][1] - gt.pt[i - 1][1]) > 0.02)))
                continue;
        groundtruth_file << names[i] << " " << (gt.pt[i][0] - start_x) * 0.96595 - 0.0221 << " " << (gt.pt[i][1] - start_y) * 0.96595 + 0.001895 << " 0.0"
                << " 0.0" << " 0.0" << " 0.0" << " 0.0" << std::endl;

        imshow("video", image);
        //cv::waitKey(1);
   }
   return 0;
}

