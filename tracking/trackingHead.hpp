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
  
};

#endif //__TRACKINGHEAD_HPP__
