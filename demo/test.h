#pragma once
#include "imgui.h"                                                                                                                                                                                    
#include "imgui_internal.h"
#include "imgInspect.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <iostream>
//#include <unistd.h>
#include <GL/glew.h>            // Initialize with gl3wInit()
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
//#include <opencv2/opencv.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../Grab_ImageCallback.h"
//#include <boost/lexical_cast.hpp>
#include "../marker_detection.h"
#include <string>
#include <pthread.h>

class demo
{
public:
    pthread_t thread_connect;
    pthread_t thread_calibrate;
    pthread_t thread_track;
    pthread_t thread_verify;
};

extern demo test;


