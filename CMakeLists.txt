cmake_minimum_required(VERSION 3.8.0)
project(hello_mvs)

add_subdirectory(thirdparty/MVS)
add_executable(Grab_ImageCallback Grab_ImageCallback.cpp)
target_link_libraries(Grab_ImageCallback MVS)
