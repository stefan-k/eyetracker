## EyeTracker
## CMakeLists file

cmake_minimum_required(VERSION 2.6)
project(Hough-Line-Tracking)

# OpenCV support
find_package(OpenCV)
include_directories(${OPENCV_INCLUDE_DIR})

# Project config
add_executable(
  et
  eyetracker.cpp
  eyetracker.h
)

target_link_libraries(
  lines
  opencv_core
  opencv_highgui
  opencv_features2d
  opencv_calib3d
  opencv_video
)
