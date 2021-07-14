#include "imgui.h"
#include "imgui_internal.h"
#include "imgInspect.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>            // Initialize with gl3wInit()
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "../Grab_ImageCallback.h"
#include "../marker_detection.h"
#include <string> 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

cv::Mat convertTo3Channels(const cv::Mat& binImg)
{
    cv::Mat three_channel = cv::Mat::zeros(binImg.rows,binImg.cols,CV_8UC3);
    std::vector<cv::Mat> channels;
    for (int i=0;i<3;i++)
    {
        channels.push_back(binImg);
    }
    merge(channels,three_channel);
    return three_channel;
}

cv::Mat convertto4(cv::Mat src)
{
    std::vector<cv::Mat> rgb3Channels(3);
    split(src, rgb3Channels);

    cv::Mat zero_mat = cv::Mat::zeros(Size(src.cols, src.rows), CV_8UC1);
    //cv::Mat roi(zero_mat, Rect(100, 2, 1, 280));
    //roi = Scalar(0, 0, 0);

    std::vector<cv::Mat> channels_4;
    channels_4.push_back(rgb3Channels[0]);
    channels_4.push_back(rgb3Channels[1]);
    channels_4.push_back(rgb3Channels[2]);
    channels_4.push_back(zero_mat);

    cv::Mat dst;
    merge(channels_4, dst);
    return dst;
}

typedef const unsigned char* byte;

byte matToBytes(cv::Mat image)
{
   int size = image.rows*image.cols;
   byte bytes[size];
   std::memcpy(bytes,image.data,size * sizeof(byte));
}

static GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID = 0;
    //glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            magFilter == GL_LINEAR_MIPMAP_NEAREST ||
            magFilter == GL_NEAREST_MIPMAP_LINEAR ||
            magFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }

    if (mat.channels() == 4)
	{
		inputColourFormat = GL_BGRA;
	}
    GLenum datatype = GL_UNSIGNED_BYTE;
	if(mat.depth() == CV_32F) datatype = GL_FLOAT;
	else if(mat.depth() == CV_64F) datatype = GL_DOUBLE;
	else if(mat.depth() == CV_32S) datatype = GL_INT;
	else if(mat.depth() == CV_16U) datatype = GL_UNSIGNED_SHORT;
	else if(mat.depth() == CV_16S) datatype = GL_SHORT;
	else if(mat.depth() == CV_8S)  datatype = GL_BYTE;

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST ||
            minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}

cv::Mat markerdetect(cv::Mat image, cv::Point2f *centerpoint)
{
    const std::string h_matrix_dir = "/home/icey/workspace/visual_tracking_system/demo/extrinsic_calibrationfile.yaml";
    const std::string setting_dir = "/home/icey/workspace/Aruco/intrinsic_calibrationfile.yaml";
    gt.intrinsic(setting_dir);
    aruco::MarkerDetector MDetector;
    MDetector.setDictionary("ARUCO");

    gt.undistort(time_img.m_image);
    gt.pnp(h_matrix_dir);
    centerpoint->x = gt.point.at<float> (0, 0);
    centerpoint->y = gt.point.at<float> (0, 1);
    std::cout << "gt.point: " << gt.point.at<float> (0, 0) << " , " << gt.point.at<float> (0, 1) << std::endl;
    std::cout << "centerpoint: " << centerpoint << std::endl;
    /*
    for (size_t i = 0; i < gt.markers.size(); i++){ 
        int id = gt.markers[i].id;
        switch ( id )
        {
            case 0:
                result_dir = "/home/icey/workspace/Aruco/result_0.txt";
                break;
            case 1:
                result_dir = "/home/icey/workspace/Aruco/result_1.txt";
                break;
            case 10:
                result_dir = "/home/icey/workspace/Aruco/result_10.txt";
                break;
            case 16:
                result_dir = "/home/icey/workspace/Aruco/result_16.txt";
                break;
            case 23:
                result_dir = "/home/icey/workspace/Aruco/result_23.txt";
                break;
        }
    }
    */
    return image;
}




