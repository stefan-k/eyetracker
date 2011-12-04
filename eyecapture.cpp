#include "eyecapture.h"

#include <iostream>

// opencv
#include "highgui.h"

//------------------------------------------------------------------------------
EyeCapture::EyeCapture(const unsigned cameraID, const bool convert_to_gray)
  : m_convert_to_gray(convert_to_gray)
{
  m_eye = new cv::VideoCapture(cameraID);

  if (!(m_eye->isOpened()))
  {
    std::cout << "[Error] cannot open video file";
  }

  // Get video information
  m_eye_width = (int)m_eye->get(CV_CAP_PROP_FRAME_WIDTH);
  m_eye_height = (int)m_eye->get(CV_CAP_PROP_FRAME_HEIGHT);

}

//------------------------------------------------------------------------------
EyeCapture::~EyeCapture()
{
  delete m_eye;
}

//------------------------------------------------------------------------------
cv::Mat EyeCapture::getFrame()
{
  *(m_eye) >> m_frame;
  if(m_convert_to_gray)
    cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2GRAY);

  return m_frame;
}
