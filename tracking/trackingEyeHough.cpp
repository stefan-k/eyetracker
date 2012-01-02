#include "trackingEyeHough.h"

#include <iostream>

// opencv
#include "highgui.h"


//------------------------------------------------------------------------------
TrackingEyeHough::TrackingEyeHough(const int eye_cam, int show_binary)
  : m_eye_cam(eye_cam), m_bw_threshold(10), m_hough_minDist(30), m_hough_dp(2),
    m_hough_param1(30), m_hough_param2(1), m_hough_minRadius(5), m_hough_maxRadius(40),
    m_show_binary(show_binary)
{
  m_eye = new EyeCapture(m_eye_cam, 1);

  // this is necessary to give the camera some time to adjust to the current
  // illumination of the scene
  //for(int i = 0; i < 10; i++)
  //{
    //m_eye->getFrame();
    //sleep(1); 
  //}

  //TrackedPupil pupils;
  //HoughCirclesPupil(pupils);

  //CallbackData callback_data;
  //callback_data.detected_positions = &pupils;
  //callback_data.pupil_to_track = &m_curr_pupil;

  //cv::imshow(INPUT_WINDOW_NAME, pupils.frame);
  //cv::setMouseCallback(INPUT_WINDOW_NAME, mouse_callback, &callback_data);
  //PAUSE;
  //
  TrackedPupil tmp;
  tmp.position.push_back(cv::Point2f(0,0));
  tmp.radius.push_back(0);
  tmp.frame = m_eye->getFrame();

  m_curr_pupil = tmp;
  m_prev_pupil = tmp;
  

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
  TrackedPupil tmp_pupil, tmp_prev_pupil;
  tmp_prev_pupil = m_prev_pupil;
  m_prev_pupil = m_curr_pupil;
  tmp_pupil = m_curr_pupil;
  HoughCirclesPupil(pupil);

  // find closest circle
  double min = std::numeric_limits<double>::max();
  //std::cout << "found " << pupil.position.size() << " pupils" << std::endl;
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

  // Do all the fancy stuff here.
  int tol = 5;
  int maxheight = m_binary_frame.rows;
  int maxwidth = m_binary_frame.cols;
  int x1 = tmp_pupil.position[0].x - tmp_pupil.radius[0] - tol < 0 ? 0 : tmp_pupil.position[0].x - tmp_pupil.radius[0] - tol;
  int y1 = tmp_pupil.position[0].y - tmp_pupil.radius[0] - tol < 0 ? 0 : tmp_pupil.position[0].y - tmp_pupil.radius[0] - tol;
  int x2 = x1 + 2*(tmp_pupil.radius[0]+tol) > maxwidth ? 2 : 2*(tmp_pupil.radius[0]+tol);
  int y2 = y1 + 2*(tmp_pupil.radius[0]+tol) > maxheight ? 2 : 2*(tmp_pupil.radius[0]+tol);
  cv::Rect roi_pupil(x1, y1, x2, y2);
  cv::Mat extracted_pupil;
  //std::cout << "bla1" << std::endl;
  extracted_pupil = m_binary_frame(roi_pupil).clone();
  //extracted_pupil = m_binary_frame.clone();
  //extracted_pupil = tmp_pupil.frame(roi_pupil).clone();
  //std::cout << "bla2" << std::endl;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(extracted_pupil, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, 
                   cv::Point(x1, y1));
  std::vector<cv::Point> hull;
  std::vector<std::vector<cv::Point> > hull_t;
  //std::cout << "bla3" << std::endl;
  if(contours.size() > 0)
  {
    cv::convexHull(contours[0], hull);
    hull_t.push_back(hull);
    //std::cout << "bla4" << std::endl;
    cv::drawContours(tmp_pupil.frame, contours, -1, cv::Scalar(255));
    //std::cout << "bla5" << std::endl;
    cv::drawContours(tmp_pupil.frame, hull_t, -1, cv::Scalar(235));
    //std::cout << "bla6" << std::endl;
  }

  // check if found circle is close enough
  if(min < 9)
  {
    m_curr_pupil.frame = tmp_pupil.frame.clone();
  }
  else
  {
    //m_curr_pupil.position[0].x = (m_curr_pupil.position[0].x + tmp_pupil.position[0].x + tmp_prev_pupil.position[0].x)/3;
    //m_curr_pupil.position[0].y = (m_curr_pupil.position[0].y + tmp_pupil.position[0].y + tmp_prev_pupil.position[0].y)/3;
    m_curr_pupil.position[0].x = (m_curr_pupil.position[0].x + tmp_pupil.position[0].x)/2;
    m_curr_pupil.position[0].y = (m_curr_pupil.position[0].y + tmp_pupil.position[0].y)/2;
    m_curr_pupil.frame = tmp_pupil.frame.clone();
  }

  //if(min < 800)
    //// pupil caught, update 
    //m_curr_pupil = tmp_pupil;
  //else
    //// pupil not caught, but still update frame
    //m_curr_pupil.frame = tmp_pupil.frame;

  return m_curr_pupil;
  
}

//------------------------------------------------------------------------------
void TrackingEyeHough::HoughCirclesPupil(TrackedPupil &pupil)
{
  cv::Mat gray, gray_blur, binary, edges;
  cv::Rect roi(1/4*640+60, 1/4*480+60, 640/2-120, 480/2-120);

  gray = m_eye->getFrame();
  gray = gray(roi).clone();
  equalizeHist(gray, gray);
  //cv::morphologyEx(gray, gray, cv::MORPH_OPEN, cv::Mat());
  //cv::morphologyEx(gray, gray, cv::MORPH_OPEN, cv::Mat());
  cv::erode(gray, gray, cv::Mat());


  cv::GaussianBlur(gray, gray_blur, cv::Size(9,9), 5, 5);
  //gray_blur = gray.clone();
  cv::threshold(gray_blur, binary, m_bw_threshold, 255, cv::THRESH_BINARY_INV);

  //for(int i = 0; i < 40; i++)
  //{
    //cv::morphologyEx(binary, binary, cv::MORPH_OPEN, cv::Mat());
    ////cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());
  //}

  //for(int i = 0; i < 1; i++)
    //cv::erode(binary, binary, cv::Mat());

  for(int i = 0; i < 2; i++)
    cv::dilate(binary, binary, cv::Mat());
    //cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, cv::Mat());

  std::vector<cv::Vec3f> circles;

  int adapt_param2 = m_hough_param2;
  while(circles.size() < 8 && adapt_param2 > 0)
  {
    cv::HoughCircles(binary, circles, CV_HOUGH_GRADIENT, m_hough_dp, 
                     m_hough_minDist, m_hough_param1, adapt_param2, m_hough_minRadius, 
                     m_hough_maxRadius);

    // decrease threshold if not enough circles are found
    adapt_param2 -= 1;
  }

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
