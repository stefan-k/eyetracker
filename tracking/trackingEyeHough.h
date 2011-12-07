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

void mouse_callback(int event, int x, int y, int flags, void* user_data);

class TrackingEyeHough
{

  EyeCapture* m_eye;
  TrackedPupil m_curr_pupil;

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

  void HoughCirclesPupil(TrackedPupil &pupil);

  double distance(const cv::Point2f pupil_to_track, const cv::Point2f found_pupil);
  
  
};

#endif //__TRACKINGEYEHOUGH_H__
