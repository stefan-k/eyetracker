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
//#define CAPTURE_HEAD
#define VIDEO_OUTPUT 1
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
  // Initialize Eye Tracker
  TrackingEyeHough eye(EYE_CAM, SHOW_BINARY);

  TrackedPupil pupil;

  cv::Mat frame;
  cv::Mat head_frame;

  cv::VideoWriter writer;
  cv::VideoWriter writer_bw;
  cv::VideoWriter writer_full;
  if(VIDEO_OUTPUT)
  {
    //writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(640, 480));
    writer.open("output_eye_init.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(200, 120));
    writer_bw.open("output_eye_init_bw.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(200, 120));
    writer_full.open("output.avi", CV_FOURCC('X','V','I','D'), 20, cv::Size(CALIBRATION_WINDOW_X, CALIBRATION_WINDOW_Y));
    if (!writer.isOpened() || !writer_bw.isOpened() || !writer_full.isOpened())
    {
      std::cout << "[Error] Oh noez! Something went wrong!" << std::endl;
      return -1;
    }
  }

  // define all trackbar values (EYE CAM)
  int bw_thresh = eye.getBwThreshold();
  int minDist = (int)eye.getHoughMinDist();
  // NOTE: DP is useless, since its mandatory to be float
  int param1 = (int)eye.getHoughParam1();
  int param2 = (int)eye.getHoughParam2();
  int minRadius = (int)eye.getHoughMinRadius();
  int maxRadius = (int)eye.getHoughMaxRadius();

#ifdef CAPTURE_HEAD
  TrackingHead head(HEAD_CAM, 0);

  // define all trackbar values (HEAD CAM)
  int h_bw_thresh = head.getBwThreshold();
  int h_minDist = (int)head.getHoughMinDist();
  // NOTE: DP is useless, since its mandatory to be float
  int h_param1 = (int)head.getHoughParam1();
  int h_param2 = (int)head.getHoughParam2();
  int h_minRadius = (int)head.getHoughMinRadius();
  int h_maxRadius = (int)head.getHoughMaxRadius();
#endif

  // this vector is used to store the last 50 pupil positions to draw the 
  // connecting lines
  std::vector<cv::Point2f> tracked_points;

  for(;;)
  {
    // get current frame and pupil position of eye camera 
    pupil = eye.getPupil();
    frame = pupil.frame.clone();

    if (frame.empty())
    {
      std::cout << "[Warning] EYE: Skipping empty frame." << std::endl;
      continue;
    }

#ifdef CAPTURE_HEAD
    // get current head frame to display
    head_frame = head.getFrame();
    if (head_frame.empty())
    {
      std::cout << "[Warning] HEAD: Skipping empty frame." << std::endl;
      continue;
    }
#endif

    // draw point in center of pupil
    cv::circle(frame, cv::Point2f(pupil.position[0].x, pupil.position[0].y), 2, cv::Scalar(255), 2);

    // draw connecting lines of the last 50 pupil positions
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
      cv::Mat vid_out(CALIBRATION_WINDOW_X,CALIBRATION_WINDOW_Y, CV_8UC3, cv::Scalar(255, 255,255));
      cv::Rect eye_roi(28,196,200,120);
      cv::Rect bw_roi(284,196,200,120);
      cv::Mat vid_out_eye;
      cv::Mat vid_out_bw;
      vid_out_eye = vid_out(eye_roi);
      vid_out_bw = vid_out(bw_roi);
      cv::Mat color;
      cv::cvtColor(frame.clone(), color, CV_GRAY2BGR);
      color.copyTo(vid_out_eye);
      writer << color;
      cv::Mat bw_frame = eye.getBinaryFrame();
      cv::cvtColor(bw_frame.clone(), color, CV_GRAY2BGR);
      color.copyTo(vid_out_bw);
      writer_bw << color;
      writer_full << vid_out;
    }
  }
  // print parameters
  eye.printParams();

  cv::Mat init_head_homography;

  cv::VideoWriter calib_writer;
  cv::VideoWriter calib_writer_eye;
  if(VIDEO_OUTPUT)
  {
    //writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(640, 480));
    calib_writer.open("output_calib.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(CALIBRATION_WINDOW_X, CALIBRATION_WINDOW_Y));
    calib_writer_eye.open("output_eye_calib.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(200, 120));
    if (!calib_writer.isOpened() || !calib_writer_eye.isOpened())
    {
      std::cout << "[Error] Oh noez! Something went wrong!" << std::endl;
      return -1;
    }
  }

  // CALIBRATION ROUTINE
  // The more points you choose, the better the homography estimation should get.
  std::vector<cv::Point2f> calibPoints;
  calibPoints.push_back(cv::Point2f(20,20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20,20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20,CALIBRATION_WINDOW_Y-20));
  calibPoints.push_back(cv::Point2f(20,CALIBRATION_WINDOW_Y-20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, 20));
  calibPoints.push_back(cv::Point2f(20, CALIBRATION_WINDOW_Y/2));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, CALIBRATION_WINDOW_Y-20));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X-20, CALIBRATION_WINDOW_Y/2));
  calibPoints.push_back(cv::Point2f(CALIBRATION_WINDOW_X/2, CALIBRATION_WINDOW_Y/2));
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

  // vector that holds all the pupil positions of the calibration routine
  std::vector<cv::Point2f> pupilPos;

  // loop through all calibration points and show one after the other
  for(int i = 0; i < calibPoints.size(); i++)
  {
    cv::Mat calibWindow(CALIBRATION_WINDOW_X, CALIBRATION_WINDOW_Y, CV_8UC1, cv::Scalar(0));
    cv::circle(calibWindow, calibPoints[i], 4, cv::Scalar(255), 2);
    cv::imshow(CALIBRATION_WINDOW_NAME, calibWindow);

    cv::Point2f pointsum(0,0);
    int j = 0;
    cv::Point2f transf_pupil;

    // get all the eye frames until user presses button
    for(;;)
    {
      pupil = eye.getPupil();

      frame = pupil.frame.clone();
      cv::imshow(EYE_WINDOW_NAME, frame);
      if(cv::waitKey(10) >= 0) break;
      j++;
      if(VIDEO_OUTPUT)
      {
        cv::Mat color_calib;
        cv::Mat color;
        cv::Mat color_calib_roi;
        cv::cvtColor(calibWindow.clone(), color_calib, CV_GRAY2BGR);
        calib_writer << color_calib;
        cv::cvtColor(frame.clone(), color, CV_GRAY2BGR);
        calib_writer_eye << color;
        cv::Rect calib_roi(284,284,200,120);
        color_calib_roi = color_calib(calib_roi);
        color.copyTo(color_calib_roi);
        writer_full << color_calib;
      }
    }

    //cv::Mat tmp_head = head.getFrame();
    //cv::Mat head_homography = head.getHomography();

    // transfer eye frame into head frame... this is just a crude estimation
    //pupil.position[0].x = 0.90*pupil.position[0].x+100;
    //pupil.position[0].y = 0.70*pupil.position[0].y+200;

    // apply current head homography
    //double z = 1;
    //transf_pupil.x = head_homography.at<double>(0,0)*pupil.position[0].x + 
                     //head_homography.at<double>(0,1)*pupil.position[0].y + 
                     //head_homography.at<double>(0,2)*z;
    //transf_pupil.y = head_homography.at<double>(1,0)*pupil.position[0].x + 
                     //head_homography.at<double>(1,1)*pupil.position[0].y + 
                     //head_homography.at<double>(1,2)*z;
    //z = head_homography.at<double>(2,0)*pupil.position[0].x + 
        //head_homography.at<double>(2,1)*pupil.position[0].y + 
        //head_homography.at<double>(2,2)*z;
    //transf_pupil.x = transf_pupil.x/z;
    //transf_pupil.y = transf_pupil.y/z;
    //z = 1;

    // this isn't really needed, just for testing purposes (transfering
    // the pupil back onto the reference frame of the head frame)
    //init_head_homography = head_homography.clone();

    transf_pupil.x = pupil.position[0].x;
    transf_pupil.y = pupil.position[0].y;
    //std::cout << " pupil x " << pupil.position[0].x 
              //<< " trans x " << transf_pupil.x
              //<< " pupil y " << pupil.position[0].y
              //<< " trans y " << transf_pupil.y << std::endl;

    pupilPos.push_back(transf_pupil);
    if(cv::waitKey(10) >= 0) break;
  }

  // estimate homography from the pupil positions and the points on the screen
  cv::Mat homography;
  //homography = cv::findHomography(pupilPos, calibPoints,  CV_RANSAC);
  homography = cv::findHomography(pupilPos, calibPoints,  CV_LMEDS);
  //homography = cv::findHomography(pupilPos, calibPoints,  0);
  //homography = cv::getPerspectiveTransform(pupilPos, calibPoints);

  // Print Matrix
  //for(int j = 0; j < 3; j++)
  //{
    //for(int k = 0; k < 3; k++)
    //{
      //std::cout << homography.at<float>(j,k) << " ";
    //}
    //std::cout << std::endl;
  //}
  cv::VideoWriter mapping_writer;
  cv::VideoWriter mapping_writer_eye;
  if(VIDEO_OUTPUT)
  {
    //writer.open("output.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(640, 480));
    mapping_writer.open("output_mapping.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(CALIBRATION_WINDOW_X, CALIBRATION_WINDOW_Y));
    mapping_writer_eye.open("output_eye_mapping.avi", CV_FOURCC('X','V','I','D'), 10, cv::Size(200, 120));
    if (!mapping_writer.isOpened() || !mapping_writer_eye.isOpened())
    {
      std::cout << "[Error] Oh noez! Something went wrong!" << std::endl;
      return -1;
    }
  }

  // MAPPING
  // Warp Eye-Image and map eyemovement onto screen
  cv::Mat frame_warped;
  for(;;)
  {
    // get current pupil and frame
    pupil = eye.getPupil();
    frame = pupil.frame.clone();
    if (frame.empty())
    {
      std::cout << "[Warning] EYE: Skipping empty frame." << std::endl;
      continue;
    }

    // get current head frame and head homography
    //cv::Mat head_frame = head.getFrame();
    //cv::Mat head_homography = head.getHomography();

    cv::Mat homography2;
    //cv::Mat head_homography2;
    //cv::Mat init_head_homography_inv;

    // testing!
    //cv::invert(init_head_homography, init_head_homography_inv);

    // multiply main homography with head homography before applying to 
    // pupil position
    //cv::gemm(homography, head_homography, 1, cv::Mat(), 0, homography2);
    homography2 = homography;
    // testing! transforms pupil from the marker frame to the head frame
    // this shows how unstable the solution is
    //cv::gemm(init_head_homography_inv, head_homography, 1, cv::Mat(), 0, head_homography2);
    
    // warp eye frame onto calibration window -> this is nice because it shows
    // that the mapping is extremely sensitive (pupil is huge, which means that 
    // very limited movement has to be tracked) and in case the homography sucks
    // it is easy to see because the warped image is totally distorted
    cv::warpPerspective(frame,frame_warped,homography2,cv::Size(CALIBRATION_WINDOW_X,CALIBRATION_WINDOW_Y));
    frame_warped = cv::Scalar(0);
    cv::Point2f new_point;
    cv::Point2f head_point;
    double z = 1;

    float x_tmp = pupil.position[0].x;
    float y_tmp = pupil.position[0].y;

    // again transform pupil position onto reference head frame
    // this is because one has to know where in the marker frame
    // the eye is...
    //pupil.position[0].x = 0.90*pupil.position[0].x+100;
    //pupil.position[0].y = 0.70*pupil.position[0].y+200;

    // apply homography
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

    // testing!
    // warp eye position in marker frame back to head frame
    //head_point.x = head_homography2.at<double>(0,0)*pupil.position[0].x + 
                   //head_homography2.at<double>(0,1)*pupil.position[0].y + 
                   //head_homography2.at<double>(0,2)*z;
    //head_point.y = head_homography2.at<double>(1,0)*pupil.position[0].x + 
                   //head_homography2.at<double>(1,1)*pupil.position[0].y + 
                   //head_homography2.at<double>(1,2)*z;
    //z = head_homography2.at<double>(2,0)*pupil.position[0].x + 
        //head_homography2.at<double>(2,1)*pupil.position[0].y + 
        //head_homography2.at<double>(2,2)*z;
    //head_point.x = head_point.x/z;
    //head_point.y = head_point.y/z;
    //z = 1;

    // draw tracked point
    cv::circle(frame_warped, new_point, 4, cv::Scalar(255), 2);

    // draw pupil position onto eye frame
    cv::circle(frame, cv::Point2f(x_tmp, y_tmp), 2, cv::Scalar(255), 2);
    // draw pupil into head frame
    //cv::circle(head_frame, cv::Point2f(head_point.x, head_point.y), 2, cv::Scalar(255), 2);
    cv::imshow(EYE_WINDOW_NAME, frame);
    cv::imshow(CALIBRATION_WINDOW_NAME, frame_warped);
    //cv::imshow(HEAD_WINDOW_NAME, head_frame);
    if(cv::waitKey(10) >= 0) break;
    if(VIDEO_OUTPUT)
    {
      cv::Mat color;
      cv::Mat color_fr;
      cv::Rect eye_roi(284,284,200,120);
      cv::Mat color_fr_roi;
      cv::cvtColor(frame.clone(), color, CV_GRAY2BGR);
      mapping_writer_eye << color;
      cv::cvtColor(frame_warped.clone(), color_fr, CV_GRAY2BGR);
      mapping_writer << color_fr;
      color_fr_roi = color_fr(eye_roi);
      color.copyTo(color_fr_roi);
      writer_full << color_fr;
    }
  }

  return 0;
}
