cmake_minimum_required(VERSION 3.8.0)
project(test)
add_compile_options(-std=c++11)
SET(CMAKE_BUILD_TYPE Release)

find_package(OpenCV 4.2 REQUIRED )
find_package(OpenGL REQUIRED )
find_package(glfw3 REQUIRED )

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIR}
  ../display/imgui
  ../display/imgui/backends
  /usr/include/SDL/
  /usr/local/include/gl3w/
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
    )
add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GL3W")
target_link_libraries(demo GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)

