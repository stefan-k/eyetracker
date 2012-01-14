#include "headcapture.h"

#include <iostream>

// opencv
#include "highgui.h"

//------------------------------------------------------------------------------
HeadCapture::HeadCapture(const unsigned cameraID, const bool convert_to_gray)
  : m_convert_to_gray(convert_to_gray)
{
  m_head = new cv::VideoCapture(cameraID);

  if (!(m_head->isOpened()))
  {
    std::cout << "[Error] cannot open video file";
  }

  // Get video information
  m_head_width = (int)m_head->get(CV_CAP_PROP_FRAME_WIDTH);
  m_head_height = (int)m_head->get(CV_CAP_PROP_FRAME_HEIGHT);

}

//------------------------------------------------------------------------------
HeadCapture::~HeadCapture()
{
  delete m_head;
}

//------------------------------------------------------------------------------
cv::Mat HeadCapture::getFrame()
{
  *(m_head) >> m_frame;
  if(m_convert_to_gray)
    cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2GRAY);

  return m_frame;
}
