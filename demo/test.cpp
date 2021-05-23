// Dear ImGui: standaaone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

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

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


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
/*
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file 
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;
    
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
    
    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
    
    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
    
    return true;
}
*/
/*
bool ImGui::ColorButtonLongTouch(const char* desc_id, const ImVec4& col, bool* bLongTouch, ImGuiColorEditFlags flags, ImVec2 size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    float default_size = GetFrameHeight();
    if (size.x == 0.0f)
        size.x = default_size;
    if (size.y == 0.0f)
        size.y = default_size;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    if ((bLongTouch != NULL) && (pressed == true) && ((flags & ImGuiButtonFlags_Repeat) == ImGuiButtonFlags_Repeat))
    {
        *bLongTouch = held & hovered;
    }

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
        bb_inner.Expand(off);
    }
    if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
    {
        float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
        RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight);
        window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_rgb_without_alpha), rounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
    }
    else
    {
        // Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
        ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;
        if (col_source.w < 1.0f)
            RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
        else
            window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding, ImDrawCornerFlags_All);
    }
    RenderNavHighlight(bb, id);
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        if (g.Style.FrameBorderSize > 0.0f)
            RenderFrameBorder(bb.Min, bb.Max, rounding);
        else
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border
    }

    // Drag and Drop Source
    // NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
    if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
        else
            SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
        ColorButtonLongTouch(desc_id, col, bLongTouch, flags);
        SameLine();
        TextEx("Color");
        EndDragDropSource();
    }

    // Tooltip
    if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
        ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

    return pressed;
}
*/
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
    GLuint textureID;
    glGenTextures(1, &textureID);

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

void excalibrate()
{

}

cv::Mat markerdetect(cv::Mat image, cv::Point2f *centerpoint)
{
    //groundtruth gt;
    const std::string h_matrix_dir = "/home/icey/workspace/Aruco/extrinsic_calibrationfile.yaml";
    std::string result_dir;
    const std::string setting_dir = "/home/icey/workspace/Aruco/intrinsic_calibrationfile.yaml";
    gt.intrinsic(setting_dir);
    aruco::MarkerDetector MDetector;
    MDetector.setDictionary("ARUCO");

    gt.undistort(grab.m_image);
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
static bool cmp(const cv::Point2f &A, const cv::Point2f &B){
    if (A.x < B.x && A.y < B.y)
        return true;
    else if(A.x < B.x && A.y >= B.y)
        return true;
    else
        return false;
}


int main(int, char**)
{
   // grabimage grab;
    // Setup window
    //Grab_image();
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

   // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "VTS", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    //bool show_demo_window = true;
    //bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::unique_lock<std::mutex> lock1(grab._mutex_locker);
    thread mythread(StartGrab);
    //mythread.join();
    //Grab_image();
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
            //ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            //thread mythread(StartGrab);
            //mythread.join();

            //static float f = 0.0f;
            //static int counter = 0;

            ImGui::Begin("Real-time visual tracking system");                          // Create a window called "Hello, world!" and append into it.
            int my_image_width = 960;
            int my_image_height = 600;
            //GLuint my_image_texture = 0;
            //bool ret = LoadTextureFromFile(grab.m_image, &my_image_texture, &my_image_width, &my_image_height);
            //ImGui::Button("connect");
            //IM_ASSERT(ret);
            ImGui::SetCursorPos(ImVec2(1000.0f, 80.0f));
            if (ImGui::Button("connect", ImVec2(90.0f, 40.0f))) {
                grab.repeat = true;
                /*
                if (!capture.read(grab.m_image)) {
                    ImGui::Text("Cannot grab a frame!");
                    break;
                }
                */
               // thread mythread(StartGrab);
                //int64_t timestamp = GetTime();
                //imwrite("./image/" + std::to_string(timestamp) + ".png", grab.m_image);
                //mythread.join();
                //cv::Mat image = imread("/home/icey/share/2021.3.25_dataset/ts_A1F3_20210325/Image_20210325142239774.bmp");
                //cv::imshow("image",image);
                //GLuint texID;
                //texID = cvMat2glTexture(image);
                //cvMatToGlTex(image, &texID);
                //bool show_demo_window = true;
                //ImGui::ShowDemoWindow(&show_demo_window);
                //cv::imshow("image", grab.m_image);
                //bool repeat = true;
                //ImGui::ColorButtonLongTouch("connect", ImVec4(1,1,1,1), &repeat, 0, ImVec2(90.0f, 40.0f));
                /*
                {
                    std::unique_lock<std::mutex> lock(grab._pic_lock);
                    GLuint my_image_texture = matToTexture(grab.m_image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                    cv::waitKey(50); 
                    ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                }
                */
                //cv::waitKey(5);
            }
            
            bool a = grab.repeat;
            if (a)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                GLuint my_image_texture = matToTexture(grab.m_image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                cv::waitKey(50);
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
            }

            //float x = 0.5;
            //float y = 0.5;
            //ImGui::SameLine(1000.0f);
            static char buf[32] = "001";
            ImGui::SetCursorPos(ImVec2(1000.0f, 40.0f));
            ImGui::InputTextWithHint("ID","Please input ID here:", buf, IM_ARRAYSIZE(buf));
            //ImGui::SetCursorPos(ImVec2(1000.0f, 80.0f));
            //ImGui::SameLine();
            //ImGui::Button("connect", ImVec2(90.0f, 40.0f));
            ImGui::SetCursorPos(ImVec2(1000.0f, 140.0f));
            if(ImGui::Button("calibration", ImVec2(90.0f, 40.0f)))
            {
                grab.calibrate = true;
                grab.calibrate_img = grab.m_image;
                cv::imwrite("./excalib.png", grab.calibrate_img);
            }
            bool b = grab.calibrate;
            if(b)
            {
                //std::unique_lock<std::mutex> lock(grab._pic_lock);
                //cv::Mat img = cv::imread("./excalib.png");
                cv::Mat img = cv::imread("/home/icey/share/extrinsic/build/data2.bmp", 0);
                cv::Mat image = cv::imread("/home/icey/图片/Screenshot from 2021-02-05 11-04-16.png", cv::IMREAD_UNCHANGED);
                //std::cout << "channel: " << img.channels() << "; type: " << img.type() << std::endl;
                //std::cout << "1" << std::endl;
                //cv::cvtColor(img, img, COLOR_GRAY2RGB);
                cv::Mat img3 = convertTo3Channels(img);
                cv::Mat img4 = convertto4(img3);
                const unsigned char* const imgbyte = img.data;
                //std::cout << "2" << std::endl;
                GLuint my_image_texture = matToTexture(img, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
                ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 50% opaque white
                
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                //ImVec2 pos = ImGui::GetCursorScreenPos();
                //groundtruth gt;
                //std::vector<cv::Point2f> four_pixel(4);
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    float region_sz = 32.0f;
                    float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                    float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                    float zoom = 4.0f;
                    if (region_x < 0.0) { region_x = 0.0; }
                    else if (region_x > (my_image_width - region_sz)) { region_x = my_image_width - region_sz; }
                    if (region_y < 0.0) { region_y = 0.0; }
                    else if (region_y > (my_image_height - region_sz)) { region_y = my_image_height - region_sz; }
                    //ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                    ImGui::Text("coord: (%.2f, %.2f)", region_x + 0.5 * region_sz, region_y + 0.5 * region_sz);
                    ImVec2 uv0 = ImVec2((region_x - zoom) / my_image_width, (region_y - zoom) / my_image_height); 
                    ImVec2 uv1 = ImVec2((region_x + region_sz - zoom) / my_image_width, (region_y + region_sz - zoom) / my_image_height);
                    ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                    //const ImVec2 pos = ImVec2( region_x + (region_sz - 1.0) / 2.0 * zoom , region_y + (region_sz - 1.0) / 2.0 * zoom );
                    //ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    //draw_list->AddCircle(ImVec2((region_x + region_sz * 0.5 - zoom), (region_y + region_sz * 0.5 - zoom)), 2.0f, 0xFF0000FF);
                    ImVec2 mouseUVCoord = ImVec2((io.MousePos.x - rc.Min.x) / rc.GetSize().x, (io.MousePos.y - rc.Min.y) / rc.GetSize().y);
                    /*
                    if (io.MouseDown[0]){
                        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(io.MousePos.x, io.MousePos.y), 2.0f, ImColor(255,255,0,255));
                        cv::Point2f four(region_x + 0.5f * region_sz, region_y + 0.5f * region_sz);
                        std::cout << "four: " << four << std::endl;
                        gt.four_pixel.push_back(four);
                        //ImGui::GetForegroundDrawList()->AddCircle(ImVec2(io.MousePos.x, io.MousePos.y), 2.0f, ImColor(255,255,0,255));
                        std::cout << "four_pixel: " << gt.four_pixel[3] << std::endl;
                    }
                    */
                    ImGui::EndTooltip();
                    if (io.MouseDown[1] && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f){
                        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(io.MousePos.x, io.MousePos.y), 5.0f, ImColor(255,255,0,255));
                        cv::Point2f four(region_x + 0.5f * region_sz, region_y + 0.5f * region_sz);
                        std::cout << "four: " << four << std::endl;
                        //gt.four_pixel.erase(gt.four_pixel.begin());
                        gt.four_pixel.push_back(four);
                        std::cout << "four_pixel: " << gt.four_pixel << std::endl;
                        //gt.four_pixel.erase(gt.four_pixel.begin());
                        //ImGui::GetForegroundDrawList()->AddCircle(ImVec2(io.MousePos.x, io.MousePos.y), 2.0f, ImColor(255,255,0,255));
                        if (gt.four_pixel[0] == cv::Point2f(0.0, 0.0))
                            gt.four_pixel.erase(gt.four_pixel.begin());
                        //std::cout << "four_pixel size: " << gt.four_pixel.size() << std::endl;
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        //std::cout << "four_pixel: " << four_pixel[i] << std::endl;
                        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(gt.four_pixel[i].x + pos.x, gt.four_pixel[i].y + pos.y), 3.0f, ImColor(255,255,0,255));
                        ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                        ImGui::Text("%d pixel selected, u:%d, v:%d", i, (int)gt.four_pixel[i].x, (int)gt.four_pixel[i].y);
                    }
                    //sort(gt.four_pixel.begin(), gt.four_pixel.end(), cmp);
                    gt.four_pixel.erase(unique(gt.four_pixel.begin(), gt.four_pixel.end()), gt.four_pixel.end());
                    if (gt.four_pixel.size() == 4 && gt.four_pixel[0] != cv::Point2f(0.0, 0.0))
                    {
                        cv::FileStorage fread ( "/home/icey/workspace/Aruco/intrinsic_calibrationfile.yaml", cv::FileStorage::READ );
                        float fx = fread["camera.fx"];
                        float fy = fread["camera.fy"];
                        float cx = fread["camera.cx"];
                        float cy = fread["camera.cy"];

                        float k1 = fread["camera.k1"];
                        float k2 = fread["camera.k2"];
                        float p1 = fread["camera.p1"];
                        float p2 = fread["camera.p2"];
                        float k3 = fread["camera.k3"];
                        gt.K = ( cv::Mat_<float> ( 3, 3 ) << fx, 0.0, cx, 0.0, fy, cy, 0.0, 0.0, 1.0);
                        gt.dist = ( cv::Mat_<float> ( 1, 5 ) << k1, k2, p1, p2, k3 );
                        cv::undistortPoints ( gt.four_pixel, gt.undist, gt.K, gt.dist, cv::noArray(), cv::noArray());
                        std::vector<cv::Point2f> point{cv::Point2f(0.0, 0.0), cv::Point2f(0.0, 1.0), cv::Point2f(1.0, 0.0), cv::Point2f(1.0, 1.0)};
                        cv::Mat H = findHomography(gt.four_pixel, point, RANSAC, 3);
                        //std::cout << std::endl << H << std::endl << std::endl << std::endl;
                        cv::FileStorage fs ( "/home/icey/workspace/visual_tracking_system/demo/extrinsic_calibrationfile.yaml", cv::FileStorage::WRITE );
                        cv::Mat cvH = ( cv::Mat_<double> ( 3, 3 ) << H.at<double> ( 0,0 ), H.at<double> ( 0,1 ), H.at<double> ( 0,2 ), 
                                        H.at<double> ( 1,0 ), H.at<double> ( 1,1 ), H.at<double> ( 1,2 ), H.at<double> ( 2,0 ), H.at<double> ( 2,1 ), H.at<double> ( 2,2 ) );
                        fs << "homograph_matrix" << cvH;
                        fs << "four_pixel" << gt.four_pixel;
                        fs.release();
                    }
                }
               /*
                std::cout << "3" << std::endl;
                ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                ImVec2 mouseUVCoord = ImVec2((io.MousePos.x - rc.Min.x) / rc.GetSize().x, (io.MousePos.y - rc.Min.y) / rc.GetSize().y);
                //mouseUVCoord.x = 1.f - mouseUVCoord.x;
                //mouseUVCoord.y = 1.f - mouseUVCoord.y;
                        
                if (io.KeyShift && io.MouseDown[0] && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f)
                {
                    int width = my_image_width;
                    int height = my_image_height;
                    ImageInspect::inspect(width, height, imgbyte, mouseUVCoord, ImVec2(9, 9));
                }*/
            }
            ImGui::SetCursorPos(ImVec2(1110.0f, 140.0f));
            if (ImGui::Button("track", ImVec2(90.0f, 40.0f)))
            {
                grab.marker = true;
            }
            bool c = grab.marker;
            if (c)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                cv::Point2f centerpoint;
                cv::Mat image = markerdetect(grab.m_image, &centerpoint);
                std::cout << "center.size()" << centerpoint << std::endl;
                //std::cout << "gt.point3: " << gt.point.at<float> (0, 0) << ", " << gt.point.at<float> (0, 1) << std::endl;
                //float x;
                //float y;
                std::cout << "1" << std::endl;
                //x = gt.point.at<float> (0, 0);
                std::cout << "1" << std::endl;
                //y = gt.point.at<float> (0, 1);
                GLuint my_image_texture = matToTexture(image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                cv::waitKey(50);
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                //float x = 1.0f;
                //int y = 2;
                //string x(std::to_string(gt.pt[0]));
                //string y(std::to_string(gt.pt[1]));
                //static float y = gt.pt[1];
                ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                ImGui::BeginChild("Scrolling");
                std::cout << "1" << std::endl;
                //float x = gt.point.at<float> (0, 0);
                //float y = gt.point.at<float> (0, 1);
                //ImGui::Text("To show the marker's location, x:%f, y:%f; Application average %.3f ms/frame (%.1f FPS)", 
                //            0.5, 0.5, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Text("To show the marker's location, x:%f, y:%f", centerpoint.x, centerpoint.y);
                std::cout << "1" << std::endl;
                ImGui::EndChild();
            }
            ImGui::SetCursorPos(ImVec2(1110.0f, 80.0f));
            //ImGui::SameLine(1110.0f);
            if(ImGui::Button("verify", ImVec2(90.0f, 40.0f)))
            {
                grab.verify = true;
            }
            bool d = grab.verify;
            if (d)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                GLuint my_image_texture = matToTexture(grab.m_image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                cv::waitKey(50);
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                ImVec2 pos = ImGui::GetCursorScreenPos();
                cv::FileStorage fread ( "/home/icey/workspace/visual_tracking_system/demo/extrinsic_calibrationfile.yaml", cv::FileStorage::READ );
                cv::Mat fourposition;
                fread["four_pixel"] >> fourposition;
                std::cout << "1" << std::endl;
                std::cout << fourposition << std::endl;
                std::vector<cv::Point2f> four_position;
                std::cout << "1" << std::endl;
                for (int i = 0; i < 8; i += 2)
                {
                    float a = fourposition.at<float>(0, i);
                    std::cout << "a: " << a << std::endl;
                    float b = fourposition.at<float>(0, i + 1);
                    cv::Point2f point = cv::Point2f(a, b);
                    std::cout << "2" << std::endl;
                    four_position.push_back(point);
                }
                std::cout << "2" << std::endl;
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[0].x + pos.x, four_position[0].y + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[1].x + pos.x, four_position[1].y + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[2].x + pos.x, four_position[2].y + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[3].x + pos.x, four_position[3].y + pos.y), 3.0f, ImColor(255,255,0,255));
            }

            /*
            ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
            ImGui::BeginChild("Scrolling");
            ImGui::Text("To show the marker's location, x:%.3f, y:%.3f", &x, &y);
            ImGui::EndChild();
            //bool show_demo_window = true;
            //ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            ImGuicounter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
*/
            ImGui::End();
        }
/*
        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
*/
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
