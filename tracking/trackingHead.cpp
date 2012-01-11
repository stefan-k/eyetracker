/**
 * @file trackingHough.h
 * @brief Captures the frames of the eye
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#include "trackingHead.hpp"

#ifndef __TRACKINGHEAD_CPP__
#define __TRACKINGHEAD_CPP__

#include <iostream>

// opencv
#include "highgui.h"


//------------------------------------------------------------------------------
TrackingHead::TrackingHead(const int head_cam, int show_binary)
  : m_head_cam(head_cam), m_bw_threshold(30), m_show_binary(show_binary)
  //: m_eye_cam(eye_cam), m_bw_threshold(5), m_hough_minDist(30), m_hough_dp(2),
    //m_hough_param1(30), m_hough_param2(1), m_hough_minRadius(10), m_hough_maxRadius(40),
    //m_show_binary(show_binary)
{
  m_head = new HeadCapture(m_head_cam, 1);

}

cv::Mat TrackingHead::getFrame()
{
  m_frame = m_head->getFrame().clone();
  cv::threshold(m_frame.clone(), m_binary_frame, m_bw_threshold, 255, cv::THRESH_BINARY);
  if(m_show_binary)
    return m_binary_frame.clone();
  else
    return m_frame.clone();
}

//------------------------------------------------------------------------------
TrackingHead::~TrackingHead()
{
}

#endif // __TRACKINGHEAD_CPP__
