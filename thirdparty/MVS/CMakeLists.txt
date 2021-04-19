cmake_minimum_required(VERSION 3.8.0)
project(MVS)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(${PROJECT_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libFormatConversion.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libGCBase_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libGenApi_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/liblog4cpp_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libLog_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMathParser_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMediaProcess.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMvCameraControl.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMVGigEVisionSDK.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMVRender.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libMvUsb3vTL.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libNodeMapData_gcc421_v3_0.so
  ${CMAKE_CURRENT_LIST_DIR}/lib/64/libXmlParser_gcc421_v3_0.so)

