/**
 * @file headcapture.h
 * @brief Captures the frames of the eye
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#ifndef __HEADCAPTURE_H__
#define __HEADCAPTURE_H__

// stl
#include <vector>

// opencv
#include <cv.h>

class HeadCapture
{
  cv::VideoCapture* m_head;
  cv::Mat m_frame;
  unsigned m_head_height;
  unsigned m_head_width;
  const bool m_convert_to_gray;

public:

  /**
   * Constructor
   * @param 
   */
  HeadCapture(const unsigned cameraID, const bool convert_to_gray);

  /**
   * Destructor
   */
  virtual ~HeadCapture();

  /**
   * getFrame
   * @param 
   */
  cv::Mat getFrame();

  unsigned getHeight()
  {
    return m_head_height;
  }

  unsigned getWidth()
  {
    return m_head_width;
  }
  
  
private:

  /**
   * disabled default constructor
   */
  HeadCapture();

  /**
   * disabled copy constructor
   */
  HeadCapture(const HeadCapture&);
  
  /**
   * disabled copy constructor
   */
  HeadCapture& operator = (const HeadCapture&);
  
};

#endif //__HEADCAPTURE_H__
