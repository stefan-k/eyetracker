## EyeTracker
## CMakeLists file

cmake_minimum_required(VERSION 2.6)
project(EyeTracker)

# OpenCV support
find_package(OpenCV)
include_directories(${OPENCV_INCLUDE_DIR})

# Project config
add_executable(
  eyetracker
  eyetracker.cpp
  eyetracker.h
)

target_link_libraries(
  eyetracker
  opencv_core
  opencv_highgui
  opencv_features2d
  opencv_calib3d
  opencv_video
)
