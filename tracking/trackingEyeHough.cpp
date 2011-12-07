#include "trackingEyeHough.h"

#include <iostream>

// opencv
#include "highgui.h"


//------------------------------------------------------------------------------
TrackingEyeHough::TrackingEyeHough()
{
  m_eye = new EyeCapture(0, 1);

  // NEEDS A TOTAL MAKEOVER!! 
  
  TrackedPupil pupils;
  TrackedPupil pupil_to_track;
  HoughCirclesPupil(pupils);

  //std::vector<cv::Point2f> detected_positions;
  //std::vector<float> radius;
  //TrackedPupil pupil_to_track;

  //cv::Mat frame, binary;
  //for(int i = 0; i < 20; i++)
    //frame = m_eye->getFrame();

  //cv::threshold(frame, binary, 70, 255, cv::THRESH_BINARY);
  //for(int i = 0; i < 40; i++)
  //{
    //cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
    //cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
  //}

  //std::vector<cv::Vec3f> circles;

  //int test = 10;
  //while(circles.size() < 3 && test > 0)
  //{
    //cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, 1, 60, 30, test, 15, 40);
    //test -= 1;
  //}

  //for(int i = 0; i < circles.size(); i++)
  //{
    //detected_positions.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    //radius.push_back(circles[i][2]);
  //}
  //pupil_to_track.frame = frame.clone();

  CallbackData callback_data;
  //callback_data.detected_positions = &detected_positions;
  //callback_data.detected_radius = &radius;
  //callback_data.pupil_to_track = &pupil_to_track;
  //callback_data.image = &frame;
  callback_data.detected_positions = &pupils;
  callback_data.pupil_to_track = &pupil_to_track;
  //callback_data.image = &(pupils.frame);

  cv::imshow(INPUT_WINDOW_NAME, pupils.frame);
  cv::setMouseCallback(INPUT_WINDOW_NAME, mouse_callback, &callback_data);
  PAUSE;
}

//------------------------------------------------------------------------------
TrackingEyeHough::~TrackingEyeHough()
{
}

TrackedPupil TrackingEyeHough::getPupil()
{
  TrackedPupil pupil;
  HoughCirclesPupil(pupil);

  return pupil;
  
}

//------------------------------------------------------------------------------
void TrackingEyeHough::HoughCirclesPupil(TrackedPupil &pupil)
{
  cv::Mat gray, binary, edges;

  gray = m_eye->getFrame();

  cv::GaussianBlur(gray, gray, cv::Size(9,9), 3, 3);
  //cv::threshold(gray, binary, 60, 255, cv::THRESH_BINARY_INV);
  cv::threshold(gray, binary, 70, 255, cv::THRESH_BINARY);

  for(int i = 0; i < 40; i++)
  {
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
  }

  std::vector<cv::Vec3f> circles;

  int test = 10;
  while(circles.size() < 3 && test > 0)
  {
    cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, 1, 60, 30, test, 15, 40);
    test -= 1;
  }

  //std::cout << "Circles found: " << circles.size() << " at test " << test << std::endl;

  //TrackedPupil pupil;
  for(int i = 0; i < circles.size(); i++)
  {
    pupil.position.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    pupil.radius.push_back(circles[i][2]);
  }
  pupil.frame = gray.clone();
}

//------------------------------------------------------------------------------
void mouse_callback(int event, int x, int y, int flags, void* user_data)
{
  switch(event)
  {
  case CV_EVENT_LBUTTONDOWN:
    {
      CallbackData *data = static_cast<CallbackData*>(user_data);

      // Find line which is nearest to click position
      cv::Point2f position;
      float radius;
      cv::Point click(x,y);
      double min = std::numeric_limits<double>::max();
      for (size_t i = 0; i < data->detected_positions->position.size(); ++i)
      {
        double d = sqrt(pow(click.x - (data->detected_positions->position.at(i)).x,2)+
                 pow(click.y - (data->detected_positions->position.at(i)).y,2));
        if (d < min)
        {
          min = d;
          position = data->detected_positions->position.at(i);
          radius = data->detected_positions->radius.at(i);
        }
      }

      //LineToTrack ltt;
      TrackedPupil pupil;
      pupil.position.push_back(position);
      pupil.radius.push_back(radius);
      data->pupil_to_track = &pupil;

      // Visualize
      cv::circle((data->detected_positions->frame), 
          cv::Point(pupil.position[0].x, pupil.position[0].y), 
          pupil.radius[0], cv::Scalar(255), 2);
      cv::imshow(INPUT_WINDOW_NAME, (data->detected_positions->frame));
    }
    break;

  default:
    break;
  }
}
