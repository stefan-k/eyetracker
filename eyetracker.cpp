/**
 * @file eyetracker.cpp
 * @brief EyeTracker
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#include "eyetracker.h"
#include "eyecapture.cpp"
#include "tracking/trackingEyeHough.cpp"

/**
 * @brief The main entry point 
 */
int main(int /*argc*/, char ** /*argv*/)
{
  TrackingEyeHough eye;
  TrackedPupil pupil;
  cv::Mat frame;

  for(;;)
  {
    pupil = eye.getPupil();
    frame = pupil.frame.clone();

    for(int i = 0; i < pupil.position.size(); i++)
    {
      cv::circle(frame, cv::Point(pupil.position[i].x, pupil.position[i].y), pupil.radius[i], cv::Scalar(255), 2);
    }

    cv::imshow("edges", frame);
    if(cv::waitKey(10) >= 0) break;
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
