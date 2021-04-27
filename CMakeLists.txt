cmake_minimum_required(VERSION 3.8.0)
project(hello_mvs)

find_package(OpenCV 4.2 REQUIRED )
find_package(Threads REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_subdirectory(thirdparty/MVS)
add_executable(GrabImage GrabImage.cpp)
target_link_libraries(GrabImage MVS ${OpenCV_LIBS} Threads::Threads)
