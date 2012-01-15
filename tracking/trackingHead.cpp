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
  m_frame_height = m_head->getHeight();
  m_frame_width = m_head->getWidth();
  m_corners.push_back(cv::Point2f(0, 0));
  m_corners.push_back(cv::Point2f(m_frame_width, 0));
  m_corners.push_back(cv::Point2f(m_frame_width, m_frame_height));
  m_corners.push_back(cv::Point2f(0, m_frame_height));
  //m_markers.push_back(cv::Point2f(0, 0));
  //m_markers.push_back(cv::Point2f(m_frame_width, 0));
  //m_markers.push_back(cv::Point2f(m_frame_width, m_frame_height));
  //m_markers.push_back(cv::Point2f(0, m_frame_height));
  //m_markers.push_back(cv::Point2f(0, 0));
  //m_markers.push_back(cv::Point2f(1, 0));
  //m_markers.push_back(cv::Point2f(1, 1));
  //m_markers.push_back(cv::Point2f(0, 1));
  m_markers.push_back(cv::Point2f(0, 0));
  m_markers.push_back(cv::Point2f(200, 0));
  m_markers.push_back(cv::Point2f(200, 200));
  m_markers.push_back(cv::Point2f(0, 200));
}

cv::Mat TrackingHead::getFrame()
{
  m_frame = m_head->getFrame().clone();
  //equalizeHist(m_frame, m_frame);
  cv::threshold(m_frame.clone(), m_binary_frame, m_bw_threshold, 255, cv::THRESH_BINARY);

  for(int i = 0; i < 8; i++)
    cv::dilate(m_binary_frame, m_binary_frame, cv::Mat());

  //HoughCirclesMarkers();
  EllipseMarkers();
  for(int i = 0; i < m_circles.size(); i++)
    cv::circle(m_frame, cv::Point2f(m_circles[i][0], m_circles[i][1]), m_circles[0][2], cv::Scalar(255), 2);

  for(int i = 0; i < m_corners.size(); i++)
  {
    double min = std::numeric_limits<double>::max();
    //double max = 0;
    for(int j = 0; j < m_circles.size(); j++)
    {
      double dist = sqrt(pow(m_corners[i].x - m_circles[j][0],2) + 
                         pow(m_corners[i].y - m_circles[j][1],2));
      if(dist < min)
      //if(dist > max)
      {
        min = dist;
        //max = dist;
        m_markers[i] = cv::Point2f(m_circles[j][0], m_circles[j][1]);
      }
    }
  }
  //std::cout << "makrers" << std::endl;
  //for(int i = 0; i < 4; i++)
  //{
    //std::cout << "x " << m_markers[i].x << " y " << m_markers[i].y << std::endl;
  //}

  //m_homography = cv::findHomography(m_markers, m_corners, 0);
  //m_homography = cv::findHomography(m_markers, m_corners, CV_RANSAC);
  //m_homography = cv::getPerspectiveTransform(m_markers, m_corners);
  m_homography = cv::getPerspectiveTransform(m_corners, m_markers);
  //m_homography = cv::findHomography(m_corners, m_markers, 0);
  // just for testing purposes
  //cv::Mat frame_warped;
  //cv::warpPerspective(m_frame,frame_warped,m_homography,cv::Size(m_frame_width,m_frame_height));
  //m_frame = frame_warped.clone();

  if(m_show_binary)
    return m_binary_frame.clone();
  else
    return m_frame.clone();
}
void TrackingHead::EllipseMarkers()
{
  cv::Mat gray, binary;
  m_head->getFrame();

  binary = m_binary_frame.clone();
  gray = m_frame.clone();

  std::vector<cv::Vec3f> circles;

  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(m_binary_frame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, 
                   cv::Point(0, 0));


  if(contours.size() == 4)
  {
    for(int i = 0; i < contours.size(); i++)
    {
      cv::RotatedRect rect_n = cv::fitEllipse(contours[i]);
      cv::ellipse(m_frame, rect_n, cv::Scalar(200));
      //circles.push_back(cv::Vec3f(rect_n[0], rect_n[1], 0);
      circles.push_back(cv::Vec3f(rect_n.center.x, rect_n.center.y, 0));
    }
    m_circles = circles;
  }
  else
  {
    if(contours.size() < 4)
      std::cout << "[HEAD TRACKING] Error: Too few markers found!" << std::endl;

    if(contours.size() > 4)
      std::cout << "[HEAD TRACKING] Error: Too many markers found!" << std::endl;
  }
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
