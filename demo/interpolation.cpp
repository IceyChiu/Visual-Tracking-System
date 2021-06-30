/*
int main(int, char**) {
   std::vector<double> X = {0.1, 0.4, 1.2, 1.8, 2.0}; // must be increasing
   std::vector<double> Y = {0.1, 0.7, 0.6, 1.1, 0.9};

   tk::spline s(X,Y);
   double x=1.5, y=s(x), deriv=s.deriv(1,x);

   printf("spline at %f is %f with derivative %f\n", x, y, deriv);
   return EXIT_SUCCESS;
}
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "/home/icey/software/spline/src/spline.h"

int main(){
    std::ifstream odomfile("/home/icey/workspace/visual_tracking_system/demo/build/latest/odometry.txt");
	if (!odomfile.is_open()){
        std::cout << "Unable to open odomfile";
		system("pause");
		exit(1);
	}
    /*
    std::ifstream vslamfile("/home/icey/share/2021.3.25_dataset/pose.txt");
	if (!vslamfile.is_open()){
        std::cout << "Unable to open vslamfile";
		system("pause");
		exit(1);
	}
    */
    std::ifstream gtfile("/home/icey/workspace/visual_tracking_system/demo/build/result.txt");
	if (!gtfile.is_open()){
        std::cout << "Unable to open groundtruth file";
        system("pause");
        exit(1);
    }
    std::vector<std::string> vec1;
    std::string temp1;
	while(getline(odomfile, temp1))    
	{
		vec1.push_back(temp1);
	}
    std::vector<int> odom_ts;
    std::vector<double> timestamp_odom;
    std::vector<double> x_value_odom;
    std::vector<double> y_value_odom;
    // std::vector<double> error;
    // std::cout <<odom读入的数据为 " << std::endl;
	for (auto it1 = vec1.begin(); it1 != vec1.end(); it1++)
	{
        //std::cout << *it << std::endl;
        std::istringstream is(*it1);                    
        std::string str;
		int pam = 0;
		while (is >> str)                          
		{
            if ( pam == 0 )                       
			{
				int l = std::stoi(str.c_str(), NULL);
				odom_ts.push_back(l);
                // std::cout << std::fixed << std::setprecision(6) << r << std::endl;
			}

			if ( pam == 1 )                       
			{
				double r = std::strtod(str.c_str(), NULL);
				timestamp_odom.push_back(r);
                // std::cout << std::fixed << std::setprecision(6) << r << std::endl;
			}

            if ( pam == 2 )
            {
                double xvalue = std::strtod(str.c_str(), NULL);
                x_value_odom.push_back(xvalue);
                // std::cout << xvalue << std::endl;
            }

            if ( pam == 3 )
            {
                double yvalue = std::strtod(str.c_str(), NULL);
                y_value_odom.push_back(yvalue);
                // std::cout << yvalue << std::endl;
             }
			pam++;
		}
	}
    /*
    std::vector<std::string> vec2;
    std::string temp2;
	while(getline(vslamfile, temp2))    
	{
		vec2.push_back(temp2);
	}
    std::vector<int> vslam_ts;
    std::vector<double> x_value_vslam;
    std::vector<double> y_value_vslam;
    // std::vector<double> error2;
    // std::cout <<odom读入的数据为 " << std::endl;
	for (auto it2 = vec2.begin(); it2 != vec2.end(); it2++)
	{
        //std::cout << *it << std::endl;
        std::istringstream is(*it2);                    
        std::string str;
		int pam = 0;
		while (is >> str)                          
		{
            if ( pam == 0 )                       
			{
				double n = std::stod(str.c_str(), NULL);
				odom_ts.push_back(n);
                // std::cout << std::fixed << std::setprecision(6) << r << std::endl;
			}

            if ( pam == 1 )
            {
                double xvalue = std::strtod(str.c_str(), NULL);
                x_value_vslam.push_back(xvalue);
                // std::cout << xvalue << std::endl;
            }

            if ( pam == 2 )
            {
                double yvalue = std::strtod(str.c_str(), NULL);
                y_value_vslam.push_back(yvalue);
                // std::cout << yvalue << std::endl;
             }
			pam++;
		}
	}
    */
    std::vector<std::string> vec3;
    std::string temp3;
	while(getline(gtfile, temp3))    
	{
		vec3.push_back(temp3);
	}
    std::vector<double> timestamp_gt;
    std::vector<double> x_value_gt;
    std::vector<double> y_value_gt;
    std::vector<double> x_new_gt;
    std::vector<double> y_new_gt;
    //double sum_error = 0.0;
    // std::cout << "odom读入的数据为 " << std::endl;
	for (auto it3 = vec3.begin(); it3 != vec3.end(); it3++)
	{
        //std::cout << *it2 << std::endl;
        std::istringstream is(*it3);                    
        std::string str;
		int pam = 0;
		while (is >> str)                          
		{
			if ( pam == 0 )                       
			{
				double r = std::strtod(str.c_str(), NULL);
				timestamp_gt.push_back(r);
                std::cout << r << std::endl;
			}
            if ( pam == 1 )
            {
                double xvalue = std::strtod(str.c_str(), NULL);
                x_value_gt.push_back(xvalue);
                // std::cout << xvalue << std::endl;
            }
            if ( pam == 2 )
            {
                double yvalue = std::strtod(str.c_str(), NULL);
                y_value_gt.push_back(yvalue);
                // std::cout << yvalue << std::endl;
             }
			pam++;
		}
	}
    tk::spline sx(timestamp_gt, x_value_gt, tk::spline::cspline_hermite);
    tk::spline sy(timestamp_gt, y_value_gt, tk::spline::cspline_hermite);

    for (int i = 0; i < timestamp_odom.size(); i++){
        double x_new = sx(timestamp_odom[i]);
        x_new_gt.push_back(x_new);
        double y_new = sy(timestamp_odom[i]);
        y_new_gt.push_back(y_new);
        //error.push_back(sqrt(pow((x_new_gt[i] - x_value_odom[i]), 2) + pow((y_new_gt[i] - y_value_odom[i]), 2)));
        //error2.push_back(sqrt(pow((x_new_gt[i] - x_value_vslam[i]), 2) + pow((y_new_gt[i] - y_value_vslam[i]), 2)));
    }
    // 写入文件
    std::string result_dir = "/home/icey/workspace/visual_tracking_system/demo/build/result_10_interpolation.txt";
    std::ofstream interpolation_file(result_dir, std::ofstream::app);
    for (int i = 0; i < timestamp_odom.size(); i++){
        //sum_error += error[i];
        interpolation_file << odom_ts[i] << " " << x_new_gt[i] << " " 
            << y_new_gt[i] << std::endl;     
    }
        /*
    std::cout << "读入的第1列数据为 " << std::endl;
	for (auto it = radius.begin(); it != radius.end(); it++)
	{
        std::cout << std::setprecision(6) << std::fixed << *it << std::endl;
	}
	//cout << "successfully to open myfile";*/
	system("pause");
	return 1;
}

