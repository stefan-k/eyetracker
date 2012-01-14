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
  : m_head_cam(head_cam), m_bw_threshold(40), m_show_binary(show_binary),
    m_hough_minDist(10), m_hough_dp(2), m_hough_param1(30), m_hough_param2(1),
    m_hough_minRadius(0), m_hough_maxRadius(20)
{
  m_head = new HeadCapture(m_head_cam, 1);

}

cv::Mat TrackingHead::getFrame()
{
  m_frame = m_head->getFrame().clone();
  //equalizeHist(m_frame, m_frame);
  cv::threshold(m_frame.clone(), m_binary_frame, m_bw_threshold, 255, cv::THRESH_BINARY);

  for(int i = 0; i < 4; i++)
    cv::dilate(m_binary_frame, m_binary_frame, cv::Mat());

  HoughCirclesMarkers();
  for(int i = 0; i < m_circles.size(); i++)
    cv::circle(m_frame, cv::Point2f(m_circles[i][0], m_circles[i][1]), m_circles[0][2], cv::Scalar(255), 2);



  if(m_show_binary)
    return m_binary_frame.clone();
  else
    return m_frame.clone();
}

void TrackingHead::HoughCirclesMarkers()
{
  cv::Mat gray, binary;
  //gray = m_head->getFrame();
  m_head->getFrame();

  //cv::threshold(gray, binary, m_bw_threshold, 255, cv::THRESH_BINARY);
  binary = m_binary_frame.clone();
  gray = m_frame.clone();

  //for(int i = 0; i < 18; i++)
    //cv::dilate(binary, binary, cv::Mat());

  std::vector<cv::Vec3f> circles;

  //int adapt_param2 = m_hough_param2;
  int count = 0;
  m_hough_param2++;
  while(circles.size() != 4 && m_hough_param2 > 0)
  {
    cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, m_hough_dp, 
                     m_hough_minDist, m_hough_param1, m_hough_param2, m_hough_minRadius, 
                     m_hough_maxRadius);

    // decrease threshold if not enough circles are found
    if(circles.size() < 4)
      m_hough_param2--;
    
    if(circles.size() > 4)
      m_hough_param2++;

    if(count > 20)
      break;
    count++;
  }

  m_circles = circles;

}

//------------------------------------------------------------------------------
TrackingHead::~TrackingHead()
{
}

#endif // __TRACKINGHEAD_CPP__
