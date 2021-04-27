cmake_minimum_required(VERSION 3.8)
project(marker_detection)
#set(OpenCV_DIR /home/icey/software/opencv/build)
#set(ARUCO_PATH /home/icey/software/aruco-3.1.12/build)

#SET(CMAKE_MODULE_PATH /home/icey/software/aruco-3.1.12/cmake )
#SET(ARUCO_INCLUDE_DIRS /home/icey/software/aruco-3.1.12/build/ )
## Compile as C++11
add_compile_options(-std=c++11)
SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGES_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(THREADS_PREFER_PTHERAD_FLAG ON)

find_package(OpenCV 4.2 REQUIRED )
find_package(Eigen3  REQUIRED)
find_package(aruco REQUIRED )
find_package(Threads REQUIRED)

include_directories(
  ${EIGEN3_INCLUDE_DIR}
  ${ARUCO_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  "/home/icey/workspace/visual_tracking_system/thirdparty/MVS/include"
)

add_executable(marker_detection src/marker_detection.cpp)
target_link_libraries(marker_detection
${OpenCV_LIBS}
${EIGEN3_LIBS}
${aruco_LIBS}
)

add_executable(GrabImage src/GrabImage.cpp)
target_link_libraries(GrabImage
${OpenCV_LIBS}
${EIGEN3_LIBS}
${aruco_LIBS}
"/home/icey/workspace/visual_tracking_system/thirdparty/MVS/include/libMvCameraControl.so"
Threads::Threads
)


