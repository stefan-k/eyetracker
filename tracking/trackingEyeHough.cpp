#include "trackingEyeHough.h"

#include <iostream>

// opencv
#include "highgui.h"


//------------------------------------------------------------------------------
TrackingEyeHough::TrackingEyeHough(const int eye_cam, int show_binary)
  : m_eye_cam(eye_cam), m_bw_threshold(50), m_hough_minDist(30), m_hough_dp(1),
    m_hough_param1(30), m_hough_param2(10), m_hough_minRadius(30), m_hough_maxRadius(40),
    m_show_binary(show_binary)
{
  m_eye = new EyeCapture(m_eye_cam, 1);

  // this is necessary to give the camera some time to adjust to the current
  // illumination of the scene
  for(int i = 0; i < 10; i++)
  {
    m_eye->getFrame();
    sleep(1); 
  }

  TrackedPupil pupils;
  HoughCirclesPupil(pupils);

  CallbackData callback_data;
  callback_data.detected_positions = &pupils;
  callback_data.pupil_to_track = &m_curr_pupil;

  cv::imshow(INPUT_WINDOW_NAME, pupils.frame);
  cv::setMouseCallback(INPUT_WINDOW_NAME, mouse_callback, &callback_data);
  PAUSE;

}

//------------------------------------------------------------------------------
TrackingEyeHough::~TrackingEyeHough()
{
}

//------------------------------------------------------------------------------
TrackedPupil TrackingEyeHough::getPupil()
{
  TrackedPupil pupil;
  // temporary pupil which is updated for every frame
  TrackedPupil tmp_pupil;
  tmp_pupil = m_curr_pupil;
  HoughCirclesPupil(pupil);

  // find closest circle
  double min = std::numeric_limits<double>::max();
  for(int i = 0; i < pupil.position.size(); i++)
  {
    double dist = distance(m_curr_pupil.position[0], pupil.position[i]);
    if(dist < min)
    {
      min = dist;
      tmp_pupil.position[0] = pupil.position[i];
      tmp_pupil.radius[0] = pupil.radius[i];
      tmp_pupil.frame = pupil.frame;
    }
  }

  // check if found circle is close enough
  if(min < 80)
    // pupil caught, update 
    m_curr_pupil = tmp_pupil;
  else
    // pupil not caught, but still update frame
    m_curr_pupil.frame = tmp_pupil.frame;

  return m_curr_pupil;
  
}

//------------------------------------------------------------------------------
void TrackingEyeHough::HoughCirclesPupil(TrackedPupil &pupil)
{
  cv::Mat gray, binary, edges;
  cv::Rect roi(1/4*640+60, 1/4*480+60, 640/2-120, 480/2-120);

  gray = m_eye->getFrame();
  gray = gray(roi).clone();


  //cv::GaussianBlur(gray, gray, cv::Size(9,9), 3, 3);
  //cv::threshold(gray, binary, 60, 255, cv::THRESH_BINARY_INV);
  cv::threshold(gray, binary, m_bw_threshold, 255, cv::THRESH_BINARY);

  //for(int i = 0; i < 40; i++)
  //{
    //cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
    ////cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
  //}
  for(int i = 0; i < 40; i++)
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());

  std::vector<cv::Vec3f> circles;

  int test = m_hough_param2;
  while(circles.size() < 3 && test > 0)
  {
    cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, m_hough_dp, 
                     m_hough_minDist, m_hough_param1, test, m_hough_minRadius, 
                     m_hough_maxRadius);

    // decrease threshold if not enough circles are found
    test -= 1;
  }

  //std::cout << "Circles found: " << circles.size() << " at test " << test << std::endl;

  // push back the found circles
  for(int i = 0; i < circles.size(); i++)
  {
    pupil.position.push_back(cv::Point2f(circles[i][0], circles[i][1]));
    pupil.radius.push_back(circles[i][2]);
  }
  if(m_show_binary)
    pupil.frame = binary.clone();
  else
    pupil.frame = gray.clone();

  m_binary_frame = binary.clone();
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
        // update someday to distance function
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
      *(data->pupil_to_track) = pupil;

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

double TrackingEyeHough::distance(const cv::Point2f pupil_to_track, 
                                  const cv::Point2f found_pupil)
{
  // Euclidian distance
  return sqrt(pow(pupil_to_track.x - found_pupil.x, 2) +
              pow(pupil_to_track.y - found_pupil.y, 2));
}

void TrackingEyeHough::printParams()
{
  std::cout << "TrackingEyeHough Parameters" << std::endl;
  std::cout << " m_bw_threshold:       " << m_bw_threshold    << std::endl;
  std::cout << " m_hough_minDist:      " << m_hough_minDist   << std::endl;
  std::cout << " m_hough_dp:           " << m_hough_dp        << std::endl;
  std::cout << " m_hough_param1:       " << m_hough_param1    << std::endl;
  std::cout << " m_hough_param2:       " << m_hough_param2    << std::endl;
  std::cout << " m_hough_minRadius:    " << m_hough_minRadius << std::endl;
  std::cout << " m_hough_maxRadius:    " << m_hough_maxRadius << std::endl;
}
