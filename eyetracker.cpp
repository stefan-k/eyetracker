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

/**
 * @brief The main entry point 
 */
int main(int /*argc*/, char ** /*argv*/)
{
  cv::Mat frame, gray, edges, binary;

  // Eye Capturer
  EyeCapture eye(0, 1);

  for(;;)
  {
    gray = eye.getFrame();
    cv::GaussianBlur(gray, gray, cv::Size(9,9), 3, 3);
    cv::threshold(gray, binary, 60, 255, cv::THRESH_BINARY_INV);
    for(int i = 0; i < 40; i++)
    {
      cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
      cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
    }
    //cv::Canny(gray, edges, 10, 40, 3);
    std::vector<cv::Vec3f> circles;
    int test = 10;
    while(circles.size() < 3 && test > 0)
    {
      cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, 1, 50, 40, test, 15, 40);
      test -= 1;
    }
    //std::cout << "Circles found: " << circles.size() << " at test " << test << std::endl;
    if(circles.size() >= 1)
    {
      cv::circle(gray, cv::Point(circles[1][0], circles[1][1]), circles[1][2], cv::Scalar(255), 2);
    }

    //for(int i = 0; i < circles.size(); i++)
    //{
      //cv::circle(gray, cv::Point(circles[i][0], circles[i][1]), circles[i][2], cv::Scalar(255), 2);
    //}
    cv::imshow("edges", gray);
    //cv::imshow("edges", edges);
    if(cv::waitKey(10) >= 0) break;
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
