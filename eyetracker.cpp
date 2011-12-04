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
  cv::VideoWriter writer;
  int video_output = 0;
  if(video_output)
  {
    writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(640, 480));
    if (!writer.isOpened())
    {
      std::cout << "[Error] Cannot encode video with fallback codec!" << std::endl;
      return -1;
    }
  }

  for(;;)
  {
    pupil = eye.getPupil();
    frame = pupil.frame.clone();

    if (frame.empty())
    {
      std::cout << "[Warning] Skipping empty frame." << std::endl;
      continue;
    }

    //for(int i = 0; i < pupil.position.size(); i++)
    for(int i = 0; i < 1; i++)
    {
      cv::circle(frame, cv::Point(pupil.position[i].x, pupil.position[i].y), pupil.radius[i], cv::Scalar(255), 2);
    }

    cv::imshow("edges", frame);
    if(cv::waitKey(10) >= 0) break;

    if(video_output)
    {
      cv::Mat color;
      cv::cvtColor(frame.clone(), color, CV_GRAY2BGR);
      writer << color;
    }
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
