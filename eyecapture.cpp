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
  {
    // YCrCb
    cv::Mat Y(m_frame.rows, m_frame.cols, CV_8UC1, cv::Scalar(0));
    cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2YCrCb);
    int from_to[] = {0, 0};
    cv::mixChannels(&m_frame, 1, &Y, 1, from_to, 1);

    // HLS
    //cv::Mat Y(m_frame.rows, m_frame.cols, CV_8UC1, cv::Scalar(0));
    //cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2HLS);
    //int from_to[] = {1, 0};
    //cv::mixChannels(&m_frame, 1, &Y, 1, from_to, 1);
    
    // HSV
    //cv::Mat Y(m_frame.rows, m_frame.cols, CV_8UC1, cv::Scalar(0));
    //cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2HSV);
    //int from_to[] = {2, 0};
    //cv::mixChannels(&m_frame, 1, &Y, 1, from_to, 1);
    cv::flip(Y, Y, 1);

    return Y.clone();
    //cv::cvtColor(m_frame.clone(), m_frame, CV_BGR2GRAY);
  }
  cv::flip(m_frame, m_frame, 1);

  return m_frame;
}
