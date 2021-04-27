#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "MvCameraControl.h"
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include "./include/marker_detection.h"
#include <thread>

using namespace cv;
using namespace std;

bool g_bExit = false;
bool g_bReset = false;
VideoWriter g_vid;
Mat m_image;
uint64_t systime;

//marker识别
void marker(cv::Mat image) {
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
    //cv::Mat image;
    float start_x;
    float start_y;

    static int i = 0;

    
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
                return;
        groundtruth_file << names[i] << " " << (gt.pt[i][0] - start_x) * 0.96595 - 0.0221 << " " << (gt.pt[i][1] - start_y) * 0.96595 + 0.001895 << " 0.0"
        << " 0.0" << " 0.0" << " 0.0" << " 0.0" << std::endl;

	++i;
        imshow("video", image);
        //cv::waitKey(1);
}

// 等待用户输入enter键来结束取流或结束程序
// wait for user to input enter to stop grabbing or end the sample program
void PressEnterToExit(void)
{
    int c;
    while ( (c = getchar()) != '\n' && c != EOF );
    fprintf( stderr, "\nPress enter to exit.\n");
    while( getchar() != '\n');
    g_bExit = true;
    sleep(1);
}

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }

    return true;
}

static void* WorkThread(void* pUser)
{
    int nRet = MV_OK;

    // ch:获取数据包大小 | en:Get payload size
    MVCC_INTVALUE stParam;
    MVCC_INTVALUE time;
    uint64_t tt1;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(pUser, "PayloadSize", &stParam);
    if (MV_OK != nRet)
    {
        printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
        return NULL;
    }

    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    unsigned char * pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
    if (NULL == pData)
    {
        return NULL;
    }
    unsigned int nDataSize = stParam.nCurValue;
     
    int i = 0;

    while(1)
    {
	    i++;
		if(g_bExit)
		{
			break;
		}
	    if (g_bReset == false) {
	    nRet = MV_CC_SetCommandValue(pUser, "GevTimestampControlReset");
	    systime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
	    g_bReset = true;
	    if (MV_OK != nRet)
		{
		    printf("MV_CC_SetCommandValue fail! nRet [%x]\n", nRet);
		}
	    }

        nRet = MV_CC_GetOneFrameTimeout(pUser, pData, nDataSize, &stImageInfo, 1000);
        //nRet = MV_CC_SetCommandValue(pUser, "GevTimestampControlLatch");
        //nRet = MV_CC_GetIntValue(pUser, "GevTimestampValue", &time);
        if (nRet == MV_OK)
        {
	    m_image = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pData);
	    //imshow("test", m_image);
	    //g_vid.write(m_image);
	    //g_vid << m_image;
	    //waitKey(1);
	    
	    //marker
	    thread mythread(marker, m_image);
	    unsigned int timestampHigh = stImageInfo.nDevTimeStampHigh;
	    unsigned int timestampLow = stImageInfo.nDevTimeStampLow; 
	    int64_t timestamp = timestampHigh * pow(2, 32) + timestampLow; 
	    imwrite("./image/" + to_string(timestamp * 10 + systime) + ".png", m_image);
	    /*
	    nRet = MV_CC_SetCommandValue(pUser, "GevTimestampControlLatch");
	    if (MV_OK != nRet)
		{
		    printf("MV_CC_SetCommandValue fail! nRet [%x]\n", nRet);
		}
	    
	    nRet = MV_CC_GetIntValue(pUser, "GevTimestampValue", &time);
	    if (MV_OK != nRet)
	    	{
	    	    printf("MV_CC_SetCommandValue fail! nRet [%x]\n", nRet);
	    	}
		*/

            printf("GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n", 
                stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
        }
        else{
            printf("No data[%x]\n", nRet);
        }
    }

    free(pData);
    return 0;
}

int main()
{
    int nRet = MV_OK;

    void* handle = NULL;

    do 
    {
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

        // 枚举设备
        // enum device
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet)
        {
            printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
            break;
        }

        if (stDeviceList.nDeviceNum > 0)
        {
            for (int i = 0; i < stDeviceList.nDeviceNum; i++)
            {
                printf("[device %d]:\n", i);
                MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (NULL == pDeviceInfo)
                {
                    break;
                } 
                PrintDeviceInfo(pDeviceInfo);            
            }  
        } 
        else
        {
            printf("Find No Devices!\n");
            break;
        }

        printf("Please Intput camera index: ");
        unsigned int nIndex = 0;
        scanf("%d", &nIndex);

        if (nIndex >= stDeviceList.nDeviceNum)
        {
            printf("Intput error!\n");
            break;
        }

        // 选择设备并创建句柄
        // select device and create handle
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
        if (MV_OK != nRet)
        {
            printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
            break;
        }

        // 打开设备
        // open device
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
            break;
        }
		
        // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
        if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
        {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            if (nPacketSize > 0)
            {
                nRet = MV_CC_SetIntValue(handle,"GevSCPSPacketSize",nPacketSize);
                if(nRet != MV_OK)
                {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!\n", nRet);
                }
            }
            else
            {
                printf("Warning: Get Packet Size fail nRet [0x%x]!\n", nPacketSize);
            }
        }
		
        // 设置触发模式为off
        // set trigger mode as off
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
        if (MV_OK != nRet)
        {
            printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
            break;
        }

        // 开始取流
        // start grab image
	
        //g_vid.open("VideoTest001.avi",CAP_ANY,CAP_PROP_FPS,Size(1920,1200),0);
        nRet = MV_CC_StartGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
            break;
        }

	nRet = MV_CC_SetBoolValue(handle, "GevAcquisitionFrameRateControlEnable", true);
	if (nRet != 0)
	{
	    printf("MV_CC_SetBoolValue fail! nRet [%x]\n", nRet); 
	}

	nRet = MV_CC_SetFloatValue(handle, "AcquisitionFrameRate", 50.00);
	if (nRet != 0)
	{
	    printf("MV_CC_SetFloatValue fail! nRet [%x]\n", nRet); 
	}

        pthread_t nThreadID;
        nRet = pthread_create(&nThreadID, NULL ,WorkThread , handle);
        if (nRet != 0)
        {
            printf("thread create failed.ret = %d\n",nRet);
            break;
        }

        PressEnterToExit();

        // 停止取流
        // end grab image
        nRet = MV_CC_StopGrabbing(handle);
	//g_vid.release();
        if (MV_OK != nRet)
        {
            printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
            break;
        }

        // 关闭设备
        // close device
        nRet = MV_CC_CloseDevice(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
            break;
        }

        // 销毁句柄
        // destroy handle
        nRet = MV_CC_DestroyHandle(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
            break;
        }
    } while (0);

    if (nRet != MV_OK)
    {
        if (handle != NULL)
        {
            MV_CC_DestroyHandle(handle);
            handle = NULL;
        }
    }

    printf("exit\n");
    return 0;
}
