#pragma once
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "./thirdparty/MVS/include/MvCameraControl.h"
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <chrono> 
#include <boost/lexical_cast.hpp>
#include <thread>
#include <fstream>
#include <stdio.h> 
#include <time.h>

using namespace cv;
using namespace std;

class Grabimage{
public:
    Mat m_image;
    //void Grab_image();
    bool g_bExit = false;
    bool g_bReset = false;
    uint64_t systime;

};
void Grab_image();
int StartGrab();
extern Grabimage grab;

int64_t GetTime(void)                                                                         
{
    struct timespec rt;
    clock_gettime(CLOCK_MONOTONIC, &rt); 
    int64_t t;
    t = (int64_t)(rt.tv_sec) * 1000000 + rt.tv_nsec / 1000;
           
    return t;
}

