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
    bool repeat;
};
void Grab_image();
int StartGrab();
extern Grabimage grab;

int64_t GetTime(void);                                                                         
