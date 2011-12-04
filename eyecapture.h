/**
 * @file eyecapture.h
 * @brief Captures the frames of the eye
 *
 * Image Processing and Pattern Recognition
 * (Bildverarbeitung und Mustererkennung)
 * Project
 */

#ifndef __EYECAPTURE_H__
#define __EYECAPTURE_H__

// stl
#include <vector>

// opencv
#include <cv.h>

class EyeCapture
{
  cv::VideoCapture* m_eye;
  //cv::VideoCapture eye;
  cv::Mat m_frame;
  unsigned m_eye_height;
  unsigned m_eye_width;
  const bool m_convert_to_gray;

public:

  /**
   * Constructor
   * @param 
   */
  EyeCapture(const unsigned cameraID, const bool convert_to_gray);

  /**
   * Destructor
   */
  virtual ~EyeCapture();

  /**
   * getFrame
   * @param 
   */
  cv::Mat getFrame();
  
  
private:

  /**
   * disabled default constructor
   */
  EyeCapture();

  /**
   * disabled copy constructor
   */
  EyeCapture(const EyeCapture&);
  
  /**
   * disabled copy constructor
   */
  EyeCapture& operator = (const EyeCapture&);
  
};

#endif //__EYECAPTURE_H__
