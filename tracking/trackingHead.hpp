/**
 * @file trackingHough.h
 * @brief Captures the frames of the eye
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#ifndef __TRACKINGHEAD_HPP__
#define __TRACKINGHEAD_HPP__

// stl
#include <vector>

// opencv
#include <cv.h>

class TrackingHead
{

  HeadCapture* m_head;
  const int m_head_cam;
  cv::Mat m_frame;
  cv::Mat m_binary_frame;
  int m_bw_threshold;
  bool m_show_binary;
  
  // HoughCircles Parameters
  double m_hough_minDist;
  double m_hough_dp;
  double m_hough_param1;
  double m_hough_param2;
  int m_hough_minRadius;
  int m_hough_maxRadius;

  std::vector<cv::Vec3f> m_circles;

public:

  /**
   * Constructor
   * @param 
   */
  TrackingHead(const int head_cam, int show_binary);

  /**
   * Destructor
   */
  virtual ~TrackingHead();

  cv::Mat getFrame();

  /**
   * Set and Get Parameters for BW Thresholding
   */
  void setBwThreshold(double threshold)
  {
    m_bw_threshold = threshold;
  }

  int getBwThreshold()
  {
    return m_bw_threshold;
  }

  /**
   * Set Parameters for HoughCircles
   */
  void setHoughMinDist(double minDist)
  {
    m_hough_minDist = minDist;
  }

  void setHoughDP(double dp)
  {
    m_hough_dp = dp;
  }

  void setHoughParam1(double param1)
  {
    m_hough_param1 = param1;
  }

  void setHoughParam2(double param2)
  {
    m_hough_param2 = param2;
  }

  void setHoughMinRadius(int minRadius)
  {
    m_hough_minRadius = minRadius;
  }

  void setHoughMaxRadius(int maxRadius)
  {
    m_hough_maxRadius = maxRadius;
  }

  /**
   * Get Parameters for HoughCircles
   */
  double getHoughMinDist()
  {
    return m_hough_minDist;
  }

  double getHoughDP()
  {
    return m_hough_dp;
  }

  double getHoughParam1()
  {
    return m_hough_param1;
  }

  double getHoughParam2()
  {
    return m_hough_param2;
  }

  int getHoughMinRadius()
  {
    return m_hough_minRadius;
  }

  int getHoughMaxRadius()
  {
    return m_hough_maxRadius;
  }

  /**
   * Get Parameters for HoughCircles
   */
  cv::Mat getBinaryFrame()
  {
    return m_binary_frame.clone();
  }

private:

  /**
   * disabled default constructor
   */
  TrackingHead();

  /**
   * disabled copy constructor
   */
  TrackingHead(const TrackingHead&);
  
  /**
   * disabled copy constructor
   */
  TrackingHead& operator = (const TrackingHead&);

  void HoughCirclesMarkers();
  
};

#endif //__TRACKINGHEAD_HPP__
