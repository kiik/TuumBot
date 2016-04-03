/** @file V4L2Camera.hpp
 *  Camera class
 *  Class for communication with the robot's cameras, based on the V4L2 API.
 *  The video capture example provided with the V4L2 API has been used as a
 *  model for the class.
 *
 *  @authors
 *  @version 0.1
 *  @date 5 December 2015
 */

#ifndef RTX_V4L2_CAMERA_H
#define RTX_V4L2_CAMERA_H

#include <string>
#include <functional>

#include <linux/videodev2.h>    // V4L2 header

#include "ImageStream.hpp"

#include "cameraConstants.hpp"    // Camera constants
#include "tuum_platform.hpp"

namespace rtx { //FIXME: namespace to rtx::hal
  struct Frame {
    unsigned char   *data;
    size_t          width;
    size_t          height;
    size_t          size;     // width * height * 3
  };
}

namespace rtx { namespace hal {

  Frame toRGB(const Frame&);

  class Camera {
  private:
    static const int CAPTURE_MAX_BUFFER = 5;

  public:
    Camera(const std::string& = gC.getStr("Vision.FirstCamera"),
            const int& = CAMERA_WIDTH,
            const int& = CAMERA_HEIGHT);
    virtual ~Camera();

    std::string getDevice() const;

    int getWidth() const;
    int getHeight() const;

    Frame getFrame();
    ImageStream* getStream();

    struct buf_info {
      int index;
      unsigned int length;
      void *start;
    };

    struct video_dev {
      int fd;
      int cap_width, cap_height;
      struct buf_info buff_info[CAPTURE_MAX_BUFFER];
      int numbuffer;
    };

    typedef std::function<void (video_dev*, v4l2_buffer*)> CaptureCallback;

  protected:
    std::string m_device;
    int m_width, m_height, m_stride;
    ImageStream *m_image;

    unsigned int frame_count;
    int frame_devisor;

    video_dev videodev;

    void vidioc_enuminput(int fd);

    void log(std::string msg) {};
    void log(char* msg) {};
    void log(const char* msg) {};

  public:
    int openDevice();
    int initCapture();
    int startCapture();
    int captureFrame(CaptureCallback);
    int stopCapture();
    void closeDevice();

  };

}};

#endif // RTX_V4L2_CAMERA_H
