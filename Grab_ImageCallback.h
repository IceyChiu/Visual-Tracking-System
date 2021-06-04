#pragma once
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "./thirdparty/MVS/include/MvCameraControl.h"
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <chrono> 
#include <boost/lexical_cast.hpp>
#include <thread>
#include <iostream>
#include <stdio.h> 
#include <time.h>
#include <mutex>

using namespace cv;
using namespace std;

class Grabimage{
public:

    //void Grab_image();
    bool g_bExit = false;
    bool g_bReset = false;
    mutex _mutex_locker;
    mutex _pic_lock;
    mutex _marker_lock;
    bool repeat;
    bool marker;
    bool calibrate;
    bool verify;
    cv::Mat calibrate_img;
    uint64_t systime_last;
};
void Grab_image();
int StartGrab();
extern Grabimage grab;
typedef struct{
    Mat m_image;
    uint64_t systime;
    } image_ts;
extern image_ts time_img;
int64_t GetTime(void);                                                                         
