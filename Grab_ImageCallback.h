#pragma once
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "./thirdparty/MVS/include/MvCameraControl.h"
#pragma once
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
    Mat m_image;
    //void Grab_image();
    bool g_bExit = false;
    bool g_bReset = false;
    uint64_t systime;
    mutex _mutex_locker;
    mutex _pic_lock;
    pthread_mutex_t lock_grab;
    bool repeat;
    bool marker;
    bool calibrate;
    bool verify;
    Mat calibrate_img;
};
void Grab_image();
int StartGrab();
extern Grabimage grab;
void PressEnterToExit();
bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
                                                                       
