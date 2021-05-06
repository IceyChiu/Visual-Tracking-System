cmake_minimum_required(VERSION 3.8.0)
project(test)
add_compile_options(-std=c++11)
SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGES_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(THREADS_PREFER_PTHERAD_FLAG ON)
#set(OpenCV_DIR /usr/local/)

find_package(OpenCV REQUIRED )
find_package(OpenGL REQUIRED )
find_package(glfw3 REQUIRED )
find_package(GLEW REQUIRED)
find_package(Threads REQUIRED)
find_package(Eigen3  REQUIRED)
find_package(aruco REQUIRED )

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${ARUCO_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ../display/imgui
  ../display/imgui/backends
  /usr/include/SDL/
  /usr/local/include/gl3w/
  "/opt/MVS/include"
)


add_executable(demo test.cpp
    ../display/imgui/imgui.cpp
    /usr/local/share/gl3w/gl3w.c
    ../display/imgui/backends/imgui_impl_opengl3.cpp
    ../display/imgui/backends/imgui_impl_glfw.cpp
    ../display/imgui/imgui_demo.cpp
    ../display/imgui/imgui_tables.cpp
    ../display/imgui/imgui_draw.cpp
    ../display/imgui/imgui_widgets.cpp
    ../GrabImage.cpp
    )
add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLEW")
message(STATUS "OpenCV_LIBS=${OpenCV_LIBRARIES}")
target_link_libraries(demo  ${OpenCV_LIBRARIES} dl pthread glfw3 "/opt/MVS/lib/64/libMvCameraControl.so" X11 Xxf86vm Xrandr Xi Xinerama Xcursor GL GLU GLEW ${aruco_LIBS})


