#include "trackingEyeHough.h"

#include <iostream>

// opencv
#include "highgui.h"

//------------------------------------------------------------------------------
TrackingEyeHough::TrackingEyeHough()
{
  m_eye = new EyeCapture(0, 1);
}

//------------------------------------------------------------------------------
TrackingEyeHough::~TrackingEyeHough()
{
}

TrackedPupil TrackingEyeHough::getPupil()
{
  cv::Mat gray, binary, edges;

  gray = m_eye->getFrame();

  cv::GaussianBlur(gray, gray, cv::Size(9,9), 3, 3);
  cv::threshold(gray, binary, 60, 255, cv::THRESH_BINARY_INV);

  for(int i = 0; i < 40; i++)
  {
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
  }

  std::vector<cv::Vec3f> circles;

  int test = 10;
  while(circles.size() < 3 && test > 0)
  {
    cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, 1, 50, 40, test, 15, 40);
    test -= 1;
  }

  //std::cout << "Circles found: " << circles.size() << " at test " << test << std::endl;

  TrackedPupil pupil;
  for(int i = 0; i < circles.size(); i++)
  {
    pupil.position.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    pupil.radius.push_back(circles[i][2]);
  }
  pupil.frame = gray.clone();

  return pupil;
  
}
