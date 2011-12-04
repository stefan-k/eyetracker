#include "trackingEyeHough.h"

#include <iostream>

// opencv
#include "highgui.h"


//------------------------------------------------------------------------------
TrackingEyeHough::TrackingEyeHough()
{
  m_eye = new EyeCapture(0, 1);

  // NEEDS A TOTAL MAKEOVER!! 

  std::vector<cv::Point2f> detected_positions;
  std::vector<float> radius;
  TrackedPupil pupil_to_track;

  cv::Mat frame, binary;
  for(int i = 0; i < 20; i++)
    frame = m_eye->getFrame();

  cv::threshold(frame, binary, 70, 255, cv::THRESH_BINARY);
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

  for(int i = 0; i < circles.size(); i++)
  {
    detected_positions.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    radius.push_back(circles[i][2]);
  }
  pupil_to_track.frame = frame.clone();

  CallbackData callback_data;
  callback_data.detected_positions = &detected_positions;
  callback_data.detected_radius = &radius;
  callback_data.pupil_to_track = &pupil_to_track;
  callback_data.image = &frame;

  cv::imshow(INPUT_WINDOW_NAME, frame);
  cv::setMouseCallback(INPUT_WINDOW_NAME, mouse_callback, &callback_data);
  PAUSE;
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

  TrackedPupil pupil;
  for(int i = 0; i < circles.size(); i++)
  {
    pupil.position.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    pupil.radius.push_back(circles[i][2]);
  }
  pupil.frame = gray.clone();
  //pupil.frame = binary.clone();

  return pupil;
  
}

void mouse_callback(int event, int x, int y, int flags, void* user_data)
{
  const uchar colors[][3] = {{255,0,0}, {45,45,230}, {10,220,250}, {180,30,180}, {160,180,30}};
  const int colors_size = sizeof(colors)/(3*sizeof(uchar));

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
      for (size_t i = 0; i < data->detected_positions->size(); ++i)
      {
        //double d = distance(click, data->detected_lines->at(i));
        double d = sqrt(pow(click.x - (data->detected_positions->at(i)).x,2)+
                 pow(click.y - (data->detected_positions->at(i)).y,2));
        if (d < min)
        {
          min = d;
          position = data->detected_positions->at(i);
          radius = data->detected_radius->at(i);
        }
      }
      // Store it for tracking
      //cv::Vec4i cart;
      //polar_to_cartesian(line, cart);
      //int ci = data->lines_to_track->size() % colors_size;
      //cv::Scalar color = cv::Scalar(colors[ci][0], colors[ci][1], colors[ci][2]);

      //LineToTrack ltt;
      TrackedPupil pupil;
      pupil.position.push_back(position);
      pupil.radius.push_back(radius);
      data->pupil_to_track = &pupil;

      // Visualize
      //cv::line(*(data->image), cv::Point(cart[0], cart[1]), cv::Point(cart[2], cart[3]), color);
      cv::circle(*(data->image), cv::Point(pupil.position[0].x, pupil.position[0].y), pupil.radius[0], cv::Scalar(255), 2);
      cv::imshow(INPUT_WINDOW_NAME, *(data->image));
    }
    break;

  default:
    break;
  }
}
