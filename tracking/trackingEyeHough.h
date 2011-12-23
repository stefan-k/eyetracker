/**
 * @file trckingHough.h
 * @brief Captures the frames of the eye
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#ifndef __TRACKINGEYEHOUGH_H__
#define __TRACKINGEYEHOUGH_H__

// stl
#include <vector>

// opencv
#include <cv.h>

typedef struct CallbackData {
  TrackedPupil *detected_positions;  
  TrackedPupil *pupil_to_track; 
} CallbackData;

// mouse callback sadly can't be within class definition (don't know why)
void mouse_callback(int event, int x, int y, int flags, void* user_data);

class TrackingEyeHough
{

  EyeCapture* m_eye;
  TrackedPupil m_curr_pupil;
  const int m_eye_cam;
  int m_show_binary;

  int m_bw_threshold;

  // HoughCircles Parameters
  double m_hough_minDist;
  double m_hough_dp;
  double m_hough_param1;
  double m_hough_param2;
  int m_hough_minRadius;
  int m_hough_maxRadius;

public:

  /**
   * Constructor
   * @param 
   */
  TrackingEyeHough(const int eye_cam, int show_binary);

  /**
   * Destructor
   */
  virtual ~TrackingEyeHough();

  /**
   * Return tracked pupils
   */
  TrackedPupil getPupil();
  
  /**
   * Print parameters
   */
  void printParams();

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
  
private:

  /**
   * disabled default constructor
   */
  TrackingEyeHough();

  /**
   * disabled copy constructor
   */
  TrackingEyeHough(const TrackingEyeHough&);
  
  /**
   * disabled copy constructor
   */
  TrackingEyeHough& operator = (const TrackingEyeHough&);

  void HoughCirclesPupil(TrackedPupil &pupil);

  double distance(const cv::Point2f pupil_to_track, const cv::Point2f found_pupil);
  
  
};

#endif //__TRACKINGEYEHOUGH_H__
