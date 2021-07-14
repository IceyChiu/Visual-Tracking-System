#include "test.h"

int main(int, char**)
{
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

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::unique_lock<std::mutex> lock1(grab._mutex_locker);
    thread mythread(StartGrab);

    std::string result_dir = "./result.txt";
    std::ofstream  groundtruth_file(result_dir, std::ofstream::app);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Real-time visual tracking system");                          // Create a window called "Hello, world!" and append into it.
            int my_image_width = 960;
            int my_image_height = 600;
            ImGui::SetCursorPos(ImVec2(1000.0f, 80.0f));
            if (ImGui::Button("connect", ImVec2(90.0f, 40.0f))) {
                grab.repeat = true;
                grab.calibrate = false;
                grab.marker = false;
                grab.verify = false;
            }
            
            bool a = grab.repeat;
            if (a)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                GLuint my_image_texture = matToTexture(time_img.m_image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                //ImGui::BeginChild("connect");
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                //ImGui::EndChild();
            }

            static char buf[32] = "001";
            ImGui::SetCursorPos(ImVec2(1000.0f, 40.0f));
            ImGui::InputTextWithHint("ID","Please input ID here:", buf, IM_ARRAYSIZE(buf));
            ImGui::SetCursorPos(ImVec2(1000.0f, 140.0f));
            if(ImGui::Button("calibration", ImVec2(90.0f, 40.0f)))
            {
                grab.repeat = false;
                grab.calibrate = true;
                grab.marker = false;
                grab.verify = false;
                grab.calibrate_img = time_img.m_image;
                cv::imwrite("./excalib.png", grab.calibrate_img);
            }
            bool b = grab.calibrate;
            if(b)
            {
                cv::Mat img = cv::imread("./excalib.png");
                //cv::Mat img = cv::imread("/home/icey/share/extrinsic/build/data2.bmp", 0);
                //cv::Mat image = cv::imread("/home/icey/图片/Screenshot from 2021-02-05 11-04-16.png", cv::IMREAD_UNCHANGED);
                //std::cout << "channel: " << img.channels() << "; type: " << img.type() << std::endl;
                //std::cout << "1" << std::endl;
                //cv::cvtColor(img, img, COLOR_GRAY2RGB);
                cv::Mat img3 = convertTo3Channels(img);
                cv::Mat img4 = convertto4(img3);
                const unsigned char* const imgbyte = img.data;
                GLuint my_image_texture = matToTexture(img, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
                ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 50% opaque white
                //ImGui::BeginChild("calibration");
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                //ImGui::EndChild();
                ImRect rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
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
                    ImGui::Text("coord: (%.2f, %.2f)", (region_x + 0.5 * region_sz) * 2, (region_y + 0.5 * region_sz) * 2);
                    ImVec2 uv0 = ImVec2((region_x - zoom) / my_image_width, (region_y - zoom) / my_image_height); 
                    ImVec2 uv1 = ImVec2((region_x + region_sz - zoom) / my_image_width, (region_y + region_sz - zoom) / my_image_height);
                    ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2((region_x + 0.5 * region_sz - zoom) / my_image_width, (region_y + 0.5 * region_sz - zoom) / my_image_height), 3.0f, ImColor(255,255,0,255));
                    ImVec2 mouseUVCoord = ImVec2((io.MousePos.x - rc.Min.x) / rc.GetSize().x, (io.MousePos.y - rc.Min.y) / rc.GetSize().y);
                    ImGui::EndTooltip();
                    if (io.MouseDown[1] && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f){
                        cv::Point2f four((region_x + 0.5f * region_sz) * 2, (region_y + 0.5f * region_sz) * 2);
                        std::cout << "four: " << four << std::endl;
                        gt.four_pixel.push_back(four);
                        std::cout << "four_pixel: " << gt.four_pixel << std::endl;
                        if (gt.four_pixel[0] == cv::Point2f(0.0, 0.0))
                            gt.four_pixel.erase(gt.four_pixel.begin());
                        //std::cout << "four_pixel size: " << gt.four_pixel.size() << std::endl;
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        //std::cout << "four_pixel: " << four_pixel[i] << std::endl;
                        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(gt.four_pixel[i].x / 2.0 + pos.x, gt.four_pixel[i].y / 2.0 + pos.y), 3.0f, ImColor(255,255,0,255));
                        ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                        
                        ImGui::BeginChild("Scrolling");
                        ImGui::Text("%d pixel selected, u:%d, v:%d", i + 1, (int)gt.four_pixel[i].x, (int)gt.four_pixel[i].y);
                        ImGui::EndChild();
                    }
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
                        cv::FileStorage fs ( "/home/icey/workspace/visual_tracking_system/demo/extrinsic_calibrationfile.yaml", cv::FileStorage::WRITE );
                        cv::Mat cvH = ( cv::Mat_<double> ( 3, 3 ) << H.at<double> ( 0,0 ), H.at<double> ( 0,1 ), H.at<double> ( 0,2 ), 
                                        H.at<double> ( 1,0 ), H.at<double> ( 1,1 ), H.at<double> ( 1,2 ), H.at<double> ( 2,0 ), H.at<double> ( 2,1 ), H.at<double> ( 2,2 ) );
                        float a = gt.four_pixel[0].x;
                        float b = gt.four_pixel[0].y;
                        float c = gt.four_pixel[1].x;
                        float d = gt.four_pixel[1].y;
                        float e = gt.four_pixel[2].x;
                        float f = gt.four_pixel[2].y;
                        float g = gt.four_pixel[3].x;
                        float h = gt.four_pixel[3].y;
                        
                        cv::Mat pix = ( cv::Mat_<float> (1, 8) << a, b, c, d, e, f, g, h);
                        fs << "homograph_matrix" << cvH;
                        fs << "four_pixel" << pix;
                        fs.release();
                        std::cout << "calibration done!" << std::endl;
                        ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                        ImGui::BeginChild("Scrolling");
                        ImGui::Text("calibration done!");
                        ImGui::Text("Homograph matrix: [ %.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f, %.9f ]", H.at<double> ( 0,0 ), H.at<double> ( 0,1 ), H.at<double> ( 0,2 ), 
                                    H.at<double> ( 1,0 ), H.at<double> ( 1,1 ), H.at<double> ( 1,2 ), H.at<double> ( 2,0 ), H.at<double> ( 2,1 ), H.at<double> ( 2,2 ));
                        ImGui::EndChild();
                    }
                }
            }
            ImGui::SetCursorPos(ImVec2(1110.0f, 140.0f));
            if (ImGui::Button("track", ImVec2(90.0f, 40.0f)))
            {
                grab.repeat = false;
                grab.calibrate = false;
                grab.marker = true;
                grab.verify = false;
            }
            bool c = grab.marker;
            if (c)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                cv::Mat image = markerdetect(time_img.m_image, &gt.centerpoint);
                std::cout << "center.size()" << gt.centerpoint << std::endl;
                if(isnan(gt.centerpoint.x) || isnan(gt.centerpoint.y))
                {
                    ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                    ImGui::BeginChild("Scrolling1");
                    ImGui::Text("Cannot detect the marker");
                    ImGui::EndChild();
                }
                else if(((abs(gt.centerpoint.x - gt.centerpoint_last.x) > 0.02) || (abs(gt.centerpoint.y - gt.centerpoint_last.y) > 0.02)) && (abs(time_img.systime - grab.systime_last) < 60 * pow(10, 6)) || (time_img.systime == grab.systime_last))
                {
                    std::cout << "flying!" << std::endl;
                }
                else{
                    gt.centerpoint_last.x = gt.centerpoint.x;
                    gt.centerpoint_last.y = gt.centerpoint.y;
                    grab.systime_last = time_img.systime;
                    // write the result into file
                    groundtruth_file << time_img.systime << " " << gt.centerpoint.x << " " << gt.centerpoint.y << " 0.0"
                        << " 0.0" << " 0.0" << " 0.0" << " 0.0" << std::endl;

                    ImGui::SetCursorPos(ImVec2(20.0f, 720.0f));
                    ImGui::BeginChild("Scrolling1");
                    ImGui::Text("To show the marker's location, x:%f, y:%f; Application average %.3f ms/frame (%.1f FPS)", 
                            gt.centerpoint.x, gt.centerpoint.y, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    //std::cout << "1" << std::endl;
                    ImGui::EndChild();
                    ImGui::SetCursorPos(ImVec2(20.0f, 680.0f));
                    if(ImGui::Button("stop", ImVec2(40.0f, 20.0f)))
                    {
                        grab.marker = false;
                        grab.repeat = true;
                    }
                }
                GLuint my_image_texture = matToTexture(image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                ImGui::SetCursorPos(ImVec2(20.0f, 40.0f));
                //ImGui::BeginChild("track");
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                //ImGui::EndChild();
            }
            ImGui::SetCursorPos(ImVec2(1110.0f, 80.0f));
            //ImGui::SameLine(1110.0f);
            if(ImGui::Button("verify", ImVec2(90.0f, 40.0f)))
            {
                grab.repeat = false;
                grab.calibrate = false;
                grab.marker = false;
                grab.verify = true;
            }
            bool d = grab.verify;
            if (d)
            {
                std::unique_lock<std::mutex> lock(grab._pic_lock);
                GLuint my_image_texture = matToTexture(time_img.m_image, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
                //cv::waitKey(50);
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
                //ImGui::BeginChild("verify");
                ImGui::ImageButton((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[0].x / 2.0 + pos.x, four_position[0].y / 2.0 + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[1].x / 2.0 + pos.x, four_position[1].y / 2.0 + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[2].x / 2.0 + pos.x, four_position[2].y / 2.0 + pos.y), 3.0f, ImColor(255,255,0,255));
                ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(four_position[3].x / 2.0 + pos.x, four_position[3].y / 2.0 + pos.y), 3.0f, ImColor(255,255,0,255));
                //ImGui::EndChild();
            }
            ImGui::End();
        }
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
