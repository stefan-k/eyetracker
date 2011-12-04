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
  std::vector<cv::Point2f> *detected_positions;  
  std::vector<float> *detected_radius;   
  TrackedPupil *pupil_to_track; 
  cv::Mat *image;                           
} CallbackData;
void mouse_callback(int event, int x, int y, int flags, void* user_data);

class TrackingEyeHough
{

  EyeCapture* m_eye;

public:

  /**
   * Constructor
   * @param 
   */
  TrackingEyeHough();

  /**
   * Destructor
   */
  virtual ~TrackingEyeHough();

  /**
   * Return tracked pupils
   */
  TrackedPupil getPupil();

  
private:

  /**
   * disabled default constructor
   */
  //TrackingEyeHough();

  /**
   * disabled copy constructor
   */
  TrackingEyeHough(const TrackingEyeHough&);
  
  /**
   * disabled copy constructor
   */
  TrackingEyeHough& operator = (const TrackingEyeHough&);
  
  //void mouse_callback(int event, int x, int y, int flags, void* user_data);
  
};

#endif //__TRACKINGEYEHOUGH_H__
