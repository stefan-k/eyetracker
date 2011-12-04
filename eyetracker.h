/**
 * @file eyetracker.h
 * @brief Types, functions and other useful stuff.
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#ifndef __EYETRACKER_H__
#define __EYETRACKER_H__


//---------------------------------------------------------------------
// Includes

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <ios>
#include <math.h>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>

//---------------------------------------------------------------------
// Type definitions
typedef struct TrackedPupil_ {
  std::vector<cv::Point2f> position;
  std::vector<float> radius;
  cv::Mat frame;
} TrackedPupil;

//---------------------------------------------------------------------
// Function declarations



//---------------------------------------------------------------------
// Macros

/**
 * @brief Name of the input window.
 */
#define INPUT_WINDOW_NAME "eye stream"

/**
 * @brief To copy MATLAB's tic/toc functionality within C++, we have
 * to initialize some variables.
 */
#define INIT_TIC_TOC \
      double _cv_timestamp; \
      double _cv_tickfrequency = (double)cv::getTickFrequency()/1000.;

/**
 * @brief Starts the timer - just like MATLAB's tic.
 */
#define TIC_ \
      _cv_timestamp = (double)cv::getTickCount();

/**
 * @brief Starts the timer - just like MATLAB's tic.
 */
#define TIC(label) \
      std::cout << "[" << label << "] ..." << std::endl; \
      _cv_timestamp = (double)cv::getTickCount();

/**
 * @brief Stops and displays the time(r) - just like MATLAB's toc.
 */
#define TOC_ \
      _cv_timestamp = (double)cv::getTickCount() - _cv_timestamp; \
      std::cout << "[Timing] Elapsed time: "  << std::fixed << std::setprecision(2) << (_cv_timestamp/_cv_tickfrequency) << " ms" << std::endl;

/**
 * @brief Same as TOC, but displaying a custom label.
 */
#define TOC(label) \
      _cv_timestamp = (double)cv::getTickCount() - _cv_timestamp; \
      std::cout << "[" << (label) << "] Elapsed time: "  << std::fixed << std::setprecision(2) << (_cv_timestamp/_cv_tickfrequency) << " ms" << std::endl;

/**
 * @brief After displaying an image, you'll often want to pause until the user hits a key.
 */
#define PAUSE \
      std::cout << "  Paused - press any key" << std::endl; \
      cv::waitKey();

/**
 * @brief Same as above, but closes the corresponding window after key press.
 */
#define PAUSE_AND_CLOSE(window_name) \
      std::cout << "  Paused - press any key" << std::endl; \
      cv::waitKey(); \
      cv::destroyWindow(window_name);

#endif // __EYETRACKER_H__
