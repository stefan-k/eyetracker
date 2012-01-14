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
#include "headcapture.cpp"
#include "tracking/trackingHead.cpp"
#include "tracking/trackingEyeHough.cpp"

#define EYE_CAM 1
#define HEAD_CAM 0
#define CAPTURE_HEAD
#define VIDEO_OUTPUT 0
#define CONVERT_TO_GRAY 1

#define SHOW_BINARY 0

// Name definitions
#define EYE_WINDOW_NAME           "EyeCam"
#define HEAD_WINDOW_NAME          "HeadCam"
#define BINARY_WINDOW_NAME        "Binary"
#define TRACKBAR_BW_THRESHOLD     "BW Threshold"
#define TRACKBAR_HOUGH_MINDIST    "Hough minDist"
#define TRACKBAR_HOUGH_DP         "Hough DP"
#define TRACKBAR_HOUGH_PARAM1     "Hough Param1"
#define TRACKBAR_HOUGH_PARAM2     "Hough Param2"
#define TRACKBAR_HOUGH_MINRADIUS  "Hough minRadius"
#define TRACKBAR_HOUGH_MAXRADIUS  "Hough maxRadius"

// CALIBRATION
#define CALIBRATION_WINDOW_Y      512
#define CALIBRATION_WINDOW_X      512
#define CALIBRATION_WINDOW_NAME   "Calibration"

/**
 * @brief The main entry point 
 */
int main(int /*argc*/, char ** /*argv*/)
{
  TrackingEyeHough eye(EYE_CAM, SHOW_BINARY);

  TrackedPupil pupil;

  cv::Mat frame;
  cv::Mat head_frame;

  cv::VideoWriter writer;
  if(VIDEO_OUTPUT)
  {
    //writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(640, 480));
    writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(200, 120));
    if (!writer.isOpened())
    {
      std::cout << "[Error] Cannot encode video with fallback codec!" << std::endl;
      return -1;
    }
  }

#ifdef CAPTURE_HEAD
  TrackingHead head(HEAD_CAM, 0);
#endif

  // define all trackbar values (EYE CAM)
  int bw_thresh = eye.getBwThreshold();
  int minDist = (int)eye.getHoughMinDist();
  // NOTE: DP is useless, since its mandatory to be float
  int param1 = (int)eye.getHoughParam1();
  int param2 = (int)eye.getHoughParam2();
  int minRadius = (int)eye.getHoughMinRadius();
  int maxRadius = (int)eye.getHoughMaxRadius();

  // define all trackbar values (HEAD CAM)
  int h_bw_thresh = head.getBwThreshold();
  int h_minDist = (int)head.getHoughMinDist();
  // NOTE: DP is useless, since its mandatory to be float
  int h_param1 = (int)head.getHoughParam1();
  int h_param2 = (int)head.getHoughParam2();
  int h_minRadius = (int)head.getHoughMinRadius();
  int h_maxRadius = (int)head.getHoughMaxRadius();

  std::vector<cv::Point2f> tracked_points;

  for(;;)
  {
    pupil = eye.getPupil();
    frame = pupil.frame.clone();
    if (frame.empty())
    {
      std::cout << "[Warning] EYE: Skipping empty frame." << std::endl;
      continue;
    }

#ifdef CAPTURE_HEAD
    head_frame = head.getFrame();
    if (head_frame.empty())
    {
      std::cout << "[Warning] HEAD: Skipping empty frame." << std::endl;
      continue;
    }
    //for testing purposes
    //cv::Mat head_homography = head.getHomography();
    //// Print Matrix
    //for(int j = 0; j < 3; j++)
    //{
      //for(int k = 0; k < 3; k++)
      //{
        //std::cout << head_homography.at<float>(j,k) << " ";
      //}
      //std::cout << std::endl;
    //}
#endif

    //cv::circle(frame, cv::Point(pupil.position[i].x, pupil.position[i].y), pupil.radius[i], cv::Scalar(255), 2);
    cv::circle(frame, cv::Point2f(pupil.position[0].x, pupil.position[0].y), 2, cv::Scalar(255), 2);

    tracked_points.push_back(pupil.position[0]);
    for(int i = (tracked_points.size() > 50) ? tracked_points.size()-50 : 1; i < tracked_points.size(); i++)
    {
      cv::line(frame, tracked_points[i-1], tracked_points[i], cv::Scalar(245), 1);
    }
    // just that it's not getting too big...
    if(tracked_points.size() > 51)
      tracked_points.clear();

    // show eye frame
    cv::imshow(EYE_WINDOW_NAME, frame);
    cv::createTrackbar(TRACKBAR_BW_THRESHOLD,    EYE_WINDOW_NAME, &bw_thresh, 255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MINDIST,   EYE_WINDOW_NAME, &minDist,   255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_PARAM1,    EYE_WINDOW_NAME, &param1,    255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_PARAM2,    EYE_WINDOW_NAME, &param2,    255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MINRADIUS, EYE_WINDOW_NAME, &minRadius,  80, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MAXRADIUS, EYE_WINDOW_NAME, &maxRadius, 100, 0, NULL);
    if(cv::waitKey(10) >= 0) break;

    // get new Trackbar values and update 
    bw_thresh = cv::getTrackbarPos(TRACKBAR_BW_THRESHOLD,    EYE_WINDOW_NAME);
    minDist   = cv::getTrackbarPos(TRACKBAR_HOUGH_MINDIST,   EYE_WINDOW_NAME);
    param1    = cv::getTrackbarPos(TRACKBAR_HOUGH_PARAM1,    EYE_WINDOW_NAME);
    param2    = cv::getTrackbarPos(TRACKBAR_HOUGH_PARAM2,    EYE_WINDOW_NAME);
    minRadius = cv::getTrackbarPos(TRACKBAR_HOUGH_MINRADIUS, EYE_WINDOW_NAME);
    maxRadius = cv::getTrackbarPos(TRACKBAR_HOUGH_MAXRADIUS, EYE_WINDOW_NAME);
    eye.setBwThreshold(bw_thresh);
    eye.setHoughMinDist(minDist);
    eye.setHoughParam1(param1);
    eye.setHoughParam2(param2);
    eye.setHoughMinRadius(minRadius);
    eye.setHoughMaxRadius(maxRadius);

    cv::imshow(BINARY_WINDOW_NAME, eye.getBinaryFrame());
    if(cv::waitKey(10) >= 0) break;

#ifdef CAPTURE_HEAD
    // update this one accordingly
    int h_param2 = (int)head.getHoughParam2();

    // show head frame
    cv::imshow(HEAD_WINDOW_NAME, head_frame);
    cv::createTrackbar(TRACKBAR_BW_THRESHOLD,    HEAD_WINDOW_NAME, &h_bw_thresh, 255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MINDIST,   HEAD_WINDOW_NAME, &h_minDist,   255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_PARAM1,    HEAD_WINDOW_NAME, &h_param1,    255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_PARAM2,    HEAD_WINDOW_NAME, &h_param2,    255, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MINRADIUS, HEAD_WINDOW_NAME, &h_minRadius,  80, 0, NULL);
    cv::createTrackbar(TRACKBAR_HOUGH_MAXRADIUS, HEAD_WINDOW_NAME, &h_maxRadius, 100, 0, NULL);
    if(cv::waitKey(10) >= 0) break;

    // get new Trackbar values and update 
    h_bw_thresh = cv::getTrackbarPos(TRACKBAR_BW_THRESHOLD,    HEAD_WINDOW_NAME);
    h_minDist   = cv::getTrackbarPos(TRACKBAR_HOUGH_MINDIST,   HEAD_WINDOW_NAME);
    h_param1    = cv::getTrackbarPos(TRACKBAR_HOUGH_PARAM1,    HEAD_WINDOW_NAME);
    h_param2    = cv::getTrackbarPos(TRACKBAR_HOUGH_PARAM2,    HEAD_WINDOW_NAME);
    h_minRadius = cv::getTrackbarPos(TRACKBAR_HOUGH_MINRADIUS, HEAD_WINDOW_NAME);
    h_maxRadius = cv::getTrackbarPos(TRACKBAR_HOUGH_MAXRADIUS, HEAD_WINDOW_NAME);
    head.setBwThreshold(h_bw_thresh);
    head.setHoughMinDist(h_minDist);
    head.setHoughParam1(h_param1);
    head.setHoughParam2(h_param2);
    head.setHoughMinRadius(h_minRadius);
    head.setHoughMaxRadius(h_maxRadius);
#endif


    if(VIDEO_OUTPUT)
    {
      cv::Mat color;
      cv::cvtColor(frame.clone(), color, CV_GRAY2BGR);
      writer << color;
    }
  }
  // print parameters
  eye.printParams();

  // CALIBRATION ROUTINE
  std::vector<cv::Point2f> calibPoints;
  calibPoints.push_back(cv::Point2f(20,20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20,20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20,CALIBRATION_WINDOW_Y-20));
  calibPoints.push_back(cv::Point2f(20,CALIBRATION_WINDOW_Y-20));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, 20));
  //calibPoints.push_back(cv::Point2f(20, CALIBRATION_WINDOW_Y/2));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, CALIBRATION_WINDOW_Y-20));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20, CALIBRATION_WINDOW_Y/2));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, CALIBRATION_WINDOW_Y/2));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/4, CALIBRATION_WINDOW_Y/2));
  //calibPoints.push_back(cv::Point2f(3*CALIBRATION_WINDOW_X/4, CALIBRATION_WINDOW_Y/2));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/4, CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, 3*CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/4, 20));
  //calibPoints.push_back(cv::Point2f(20, CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/4, CALIBRATION_WINDOW_Y-20));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20, CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(3*CALIBRATION_WINDOW_X/4, 20));
  //calibPoints.push_back(cv::Point2f(20, 3*CALIBRATION_WINDOW_Y/4));
  //calibPoints.push_back(cv::Point2f(3*CALIBRATION_WINDOW_X/4, CALIBRATION_WINDOW_Y-20));
  //calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20, 3*CALIBRATION_WINDOW_Y/4));
  std::vector<cv::Point2f> pupilPos;

  for(int i = 0; i < calibPoints.size(); i++)
  {
    cv::Mat calibWindow(CALIBRATION_WINDOW_X, CALIBRATION_WINDOW_Y, CV_8UC1, cv::Scalar(0));
    cv::circle(calibWindow, calibPoints[i], 4, cv::Scalar(255), 2);
    cv::imshow(CALIBRATION_WINDOW_NAME, calibWindow);
    cv::Point2f pointsum(0,0);
    int j = 0;
    cv::Point2f transf_pupil;
    for(;;)
    {
      pupil = eye.getPupil();


      frame = pupil.frame.clone();
      cv::imshow(EYE_WINDOW_NAME, frame);
      //std::cout << pupil.position[0].x << " " << pupil.position[0].y << std::endl;
      //pointsum += pupil.position[0];
      //frame = pupil.frame.clone();
      if(cv::waitKey(10) >= 0) break;
      j++;
    }
    head.getFrame();
    //if(cv::waitKey(40) >= 0) break;
    cv::Mat head_homography = head.getHomography();
    //cv::invert(head_homography, head_homography);
    double z = 1;
    transf_pupil.x = head_homography.at<double>(0,0)*pupil.position[0].x + 
                     head_homography.at<double>(0,1)*pupil.position[0].y + 
                     head_homography.at<double>(0,2)*z;
    transf_pupil.y = head_homography.at<double>(1,0)*pupil.position[0].x + 
                     head_homography.at<double>(1,1)*pupil.position[0].y + 
                     head_homography.at<double>(1,2)*z;
    z = head_homography.at<double>(2,0)*pupil.position[0].x + 
        head_homography.at<double>(2,1)*pupil.position[0].y + 
        head_homography.at<double>(2,2)*z;
    transf_pupil.x = transf_pupil.x/z;
    transf_pupil.y = transf_pupil.y/z;
    z = 1;
    //pupilPos.push_back(cv::Point2f(pointsum.x/j, pointsum.y/j));
    //pupilPos.push_back(pupil.position[0]);
    //cv::perspectiveTransform(cv::Point2f(pupil.position[0].x,pupil.position[0].y), transf_pupil, head_homography);
    pupilPos.push_back(transf_pupil);
  }

  cv::Mat homography;
  //homography = cv::findHomography(pupilPos, calibPoints,  CV_RANSAC);
  //homography = cv::findHomography(pupilPos, calibPoints,  CV_LMEDS);
  //homography = cv::findHomography(pupilPos, calibPoints,  0);
  homography = cv::getPerspectiveTransform(pupilPos, calibPoints);

  // Print Matrix
  for(int j = 0; j < 3; j++)
  {
    for(int k = 0; k < 3; k++)
    {
      std::cout << homography.at<float>(j,k) << " ";
    }
    std::cout << std::endl;
  }

  // MAPPING
  // Warp Eye-Image and map eyemovement onto screen
  cv::Mat frame_warped;
  for(;;)
  {
    pupil = eye.getPupil();
    frame = pupil.frame.clone();
    if (frame.empty())
    {
      std::cout << "[Warning] EYE: Skipping empty frame." << std::endl;
      continue;
    }

    cv::Mat head_frame = head.getFrame();
    cv::Mat head_homography = head.getHomography();

    cv::Mat homography2;


    //cv::invert(head_homography,head_homography);
    cv::gemm(homography, head_homography, 1, cv::Mat(), 0, homography2, 0);
    //cv::gemm(head_homography, homography, 1, cv::Mat(), 0, homography2, 0);
    //cv::warpPerspective(frame,frame_warped,homography,frame.size());
    //cv::warpPerspective(frame,frame_warped,homography,cv::Size(CALIBRATION_WINDOW_X,CALIBRATION_WINDOW_Y), cv::INTER_CUBIC);
    cv::warpPerspective(frame,frame_warped,homography2,cv::Size(CALIBRATION_WINDOW_X,CALIBRATION_WINDOW_Y));
    cv::Point2f new_point;
    double z = 1;

    //new_point.x = homography.at<double>(0,0)*pupil.position[0].x + 
                  //homography.at<double>(0,1)*pupil.position[0].y + 
                  //homography.at<double>(0,2)*z;
    //new_point.y = homography.at<double>(1,0)*pupil.position[0].x + 
                  //homography.at<double>(1,1)*pupil.position[0].y + 
                  //homography.at<double>(1,2)*z;
    //z = homography.at<double>(2,0)*pupil.position[0].x + 
        //homography.at<double>(2,1)*pupil.position[0].y + 
        //homography.at<double>(2,2)*z;
    //new_point.x = new_point.x/z;
    //new_point.y = new_point.y/z;
    //z = 1;

    //new_point.x = head_homography.at<double>(0,0)*new_point.x + 
                  //head_homography.at<double>(0,1)*new_point.y + 
                  //head_homography.at<double>(0,2)*z;
    //new_point.y = head_homography.at<double>(1,0)*new_point.x + 
                  //head_homography.at<double>(1,1)*new_point.y + 
                  //head_homography.at<double>(1,2)*z;
    //z = head_homography.at<double>(2,0)*new_point.x + 
               //head_homography.at<double>(2,1)*new_point.y + 
               //head_homography.at<double>(2,2)*z;
    //new_point.x = new_point.x/z;
    //new_point.y = new_point.y/z;


    // multiply by frame homography first, then by eyehomography
    //new_point.x = head_homography.at<double>(0,0)*pupil.position[0].x + 
                  //head_homography.at<double>(0,1)*pupil.position[0].y + 
                  //head_homography.at<double>(0,2)*z;
    //new_point.y = head_homography.at<double>(1,0)*pupil.position[0].x + 
                  //head_homography.at<double>(1,1)*pupil.position[0].y + 
                  //head_homography.at<double>(1,2)*z;
    //z = head_homography.at<double>(2,0)*pupil.position[0].x + 
        //head_homography.at<double>(2,1)*pupil.position[0].y + 
        //head_homography.at<double>(2,2)*z;
    //new_point.x = new_point.x/z;
    //new_point.y = new_point.y/z;

    //cv::multiply(homography, head_homography, homography);
    std::cout << "new matrix" << std::endl;
    for(int j = 0; j < 3; j++)
    {
      for(int k = 0; k < 3; k++)
      {
        //std::cout << homography2.at<float>(j,k) << " ";
        std::cout << head_homography.at<float>(j,k) << " ";
      }
      std::cout << std::endl;
    }

    new_point.x = homography2.at<double>(0,0)*pupil.position[0].x + 
                  homography2.at<double>(0,1)*pupil.position[0].y + 
                  homography2.at<double>(0,2)*z;
    new_point.y = homography2.at<double>(1,0)*pupil.position[0].x + 
                  homography2.at<double>(1,1)*pupil.position[0].y + 
                  homography2.at<double>(1,2)*z;
    z = homography2.at<double>(2,0)*pupil.position[0].x + 
        homography2.at<double>(2,1)*pupil.position[0].y + 
        homography2.at<double>(2,2)*z;
    new_point.x = new_point.x/z;
    new_point.y = new_point.y/z;
    z = 1;

    //std::cout << "x " << new_point.x << " y " << new_point.y << std::endl;

    //new_point.x = homography.at<double>(0,0)*pupil.position[0].x + homography.at<double>(0,1)*pupil.position[0].y + homography.at<double>(0,2)*1;
    //new_point.y = homography.at<double>(1,0)*pupil.position[0].x + homography.at<double>(1,1)*pupil.position[0].y + homography.at<double>(1,2)*1;
    //z = homography.at<double>(2,0)*pupil.position[0].x + homography.at<double>(2,1)*pupil.position[0].y + homography.at<double>(2,2)*1;
    //new_point.x = new_point.x/z;
    //new_point.y = new_point.y/z;

    cv::circle(frame_warped, new_point, 4, cv::Scalar(255), 2);

    cv::circle(frame, cv::Point2f(pupil.position[0].x, pupil.position[0].y), 2, cv::Scalar(255), 2);
    cv::imshow(EYE_WINDOW_NAME, frame);
    cv::imshow(CALIBRATION_WINDOW_NAME, frame_warped);
    cv::imshow(HEAD_WINDOW_NAME, head_frame);
    if(cv::waitKey(10) >= 0) break;
  }




  return 0;
}
