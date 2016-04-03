/** @file V4L2Camera.hpp
 *  Camera class
 *  Class for communication with the robot's cameras, based on the V4L2 API.
 *  The video capture example provided with the V4L2 API has been used as a
 *  model for the class.
 *
 *  @authors
 *  @version 0.1
 */

#include <fcntl.h>              // File control operations (open)
#include <sys/stat.h>           // File characteristics header (stat)
#include <sys/ioctl.h>          // I/O control device header (ioctl)
#include <sys/mman.h>           // Memory management declarations (mmap)
#include <unistd.h>             // OS API header (close)
#include <cstring>              // C string header (memset, strerror)
#include <stdexcept>            // Exception header (runtime_error)

#include <iostream> // TODO: Remove

#include "V4L2Camera.hpp"       // The class header

namespace rtx { namespace hal {

  // Macro to set the memory of a variable to zero
  #define CLEAR(x) memset(&(x), 0, sizeof(x))

  #define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

  static void convertYCbCrtoRGB(const unsigned char *src, unsigned char *dest,
                                 int width, int height)
  {
    int j;
    while (--height >= 0) {
      for (j = 0; j < width; ++j) {
        // Y:  src[0]
        // Cb: src[1]
        // Cr: src[2]
        // Red
        *dest++ = CLIP(1.164 * (src[0] - 16) + 1.596 * (src[2] - 128));
        // Green
        *dest++ = CLIP(1.164 * (src[0] - 16) - 0.813 * (src[2] - 128) - 0.391 * (src[1] - 128));
        // Blue
        *dest++ = CLIP(1.164 * (src[0] - 16) + 2.018 * (src[1] - 128));
        src += 3;
      }
    }
  }

  Frame toRGB(const Frame &frame) {
    Frame rgbFrame;
    rgbFrame.data = (unsigned char *) malloc(frame.size * sizeof(char));
    std::copy(frame.data, frame.data + frame.size, rgbFrame.data);
    rgbFrame.width = frame.width;
    rgbFrame.height = frame.height;
    rgbFrame.size = frame.size;
    convertYCbCrtoRGB((unsigned char *) frame.data,
                             rgbFrame.data,
                             rgbFrame.width,
                             rgbFrame.height);
    return rgbFrame;
  }

  static void formatFrame(const unsigned char *source, unsigned char *destination,
                          int width, int height, int stride) {
    while (--height >= 0) {
      for (int i = 0; i < width - 1; i += 2) {
        for (int j = 0; j < 2; ++j) {
          *destination++ = source[j * 2];
          *destination++ = source[1];
          *destination++ = source[3];
        }
        source += 4;
      }
      source += stride - width * 2;
    }
  }

  Camera::Camera(const std::string &device, const int &width, const int &height):
    m_device(device), m_width(width), m_height(height),
    frame_count(0), frame_devisor(0)
  {
    videodev.fd = -1;
    m_image = new ImageStream(640, 480);
  }

  Camera::~Camera()
  {
    closeDevice();
    if(m_image) delete m_image;
  }

  std::string Camera::getDevice() const { return m_device; }

  int Camera::getWidth() const { return m_width; }
  int Camera::getHeight() const { return m_height; }

  ImageStream* Camera::getStream() { return m_image; }

  void Camera::vidioc_enuminput(int fd)
  {
      int err;
      struct v4l2_input input;
      memset(&input, 0, sizeof(input));
      input.index = 0;
      while ((err = ioctl(fd, VIDIOC_ENUMINPUT, &input)) == 0) {
        /*
          qDebug() << "input name =" << (char *)input.name
                   << " type =" << input.type
                   << " status =" << input.status
                   << " std =" << input.std;
        */
          input.index++;
      }
  }

  int Camera::openDevice() {
    if (videodev.fd > 0) return 0;

    int err;
    int fd = open(m_device.c_str(), O_RDWR);
    if (fd < 0) {
        log("Device open failed!");
        return fd;
    }
    videodev.fd = fd;

    struct v4l2_capability cap;
    if ((err = ioctl(fd, VIDIOC_QUERYCAP, &cap)) < 0) {
        log("VIDIOC_QUERYCAP error");
        goto err1;
    }

    /*
    qDebug() << "card =" << (char *)cap.card
             << " driver =" << (char *)cap.driver
             << " bus =" << (char *)cap.bus_info;
    */

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        log("Capable of capture");
    else {
        log("Not capable of capture");
        goto err1;
    }

    if (cap.capabilities & V4L2_CAP_STREAMING)
        log("Capable of streaming");
    else {
        log("Not capable of streaming");
        goto err1;
    }

    if ((err = initCapture()) < 0) goto err1;

    struct v4l2_requestbuffers reqbuf;
    reqbuf.count = 5;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    if ((err = ioctl(fd, VIDIOC_REQBUFS, &reqbuf)) < 0) {
        log("Cannot allocate memory");
        goto err1;
    }
    videodev.numbuffer = reqbuf.count;
    log("buffer actually allocated x"); //FIXME: << reqbuf.count;

    uint i;
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    for (i = 0; i < reqbuf.count; i++) {
      buf.type = reqbuf.type;
      buf.index = i;
      buf.memory = reqbuf.memory;
      err = ioctl(fd, VIDIOC_QUERYBUF, &buf);
      //Q_ASSERT(err == 0);

      videodev.buff_info[i].length = buf.length;
      videodev.buff_info[i].index = i;
      videodev.buff_info[i].start =
              (uchar *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

      //Q_ASSERT(videodev.buff_info[i].start != MAP_FAILED);

      memset((void *) videodev.buff_info[i].start, 0x80,
             videodev.buff_info[i].length);

      err = ioctl(fd, VIDIOC_QBUF, &buf);
      //Q_ASSERT(err == 0);
    }

    return 0;

    err1:
    close(fd);
    return err;
  }

  int Camera::initCapture() {
    int err, fd = videodev.fd;

    struct v4l2_dbg_chip_info chip;
    if ((err = ioctl(fd, VIDIOC_DBG_G_CHIP_INFO, &chip)) < 0)
      log("VIDIOC_DBG_G_CHIP_INFO error ");
    else
      log("chip info x"); //FIXME: << chip.name;

    bool support_fmt;
    struct v4l2_fmtdesc ffmt;
    memset(&ffmt, 0, sizeof(ffmt));
    ffmt.index = 0;
    ffmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    support_fmt = false;
    while ((err = ioctl(fd, VIDIOC_ENUM_FMT, &ffmt)) == 0) {
      log("fmtxx"); //FIXME: << ffmt.pixelformat << (char *)ffmt.description;
      if (ffmt.pixelformat == V4L2_PIX_FMT_YUYV) support_fmt = true;
      ffmt.index++;
    }
    if (!support_fmt) {
      log("V4L2_PIX_FMT_YUYV is not supported by this camera");
      return -1;
    }

    bool support_640x480;
    struct v4l2_frmsizeenum fsize;
    memset(&fsize, 0, sizeof(fsize));
    fsize.index = 0;
    fsize.pixel_format = V4L2_PIX_FMT_YUYV;
    support_640x480 = false;
    while ((err = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0) {
      log("frame size xx"); //FIXME: << fsize.discrete.width << fsize.discrete.height;
      if (fsize.discrete.width == 640 && fsize.discrete.height == 480)
          support_640x480 = true;
      fsize.index++;
    }
    if (!support_640x480) {
      log("frame size 640x480 is not supported by this camera");
      return -1;
    }

    vidioc_enuminput(fd);

    int index;
    if ((err = ioctl(fd, VIDIOC_G_INPUT, &index)) < 0)
      log("VIDIOC_G_INPUT fail x"); //FIXME: << errno;
    else
      log("current input index = x"); //FIXME: << index;

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ((err = ioctl(fd, VIDIOC_G_FMT, &fmt)) < 0)
      log("VIDIOC_G_FMT fail x"); //FIXME: << errno;
    else {
      /*
      qDebug() << "fmt width =" << fmt.fmt.pix.width
               << " height =" << fmt.fmt.pix.height
               << " pfmt =" << fmt.fmt.pix.pixelformat;
               */
    }

    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    m_stride = fmt.fmt.pix.bytesperline;
    if ((err = ioctl(fd, VIDIOC_S_FMT, &fmt)) < 0)
      log("VIDIOC_S_FMT fail x"); //FIXME: << errno;
    else
      log("VIDIOC_S_FMT success");

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ((err = ioctl(fd, VIDIOC_G_FMT, &fmt)) < 0)
      log("VIDIOC_G_FMT fail x"); //FIXME: << errno;
    else {
      /*
        qDebug() << "fmt width =" << fmt.fmt.pix.width
                 << " height =" << fmt.fmt.pix.height
                 << " pfmt =" << fmt.fmt.pix.pixelformat;
      */
    }

    assert(fmt.fmt.pix.width == 640);
    assert(fmt.fmt.pix.height == 480);
    assert(fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV);

    videodev.cap_width = fmt.fmt.pix.width;
    videodev.cap_height = fmt.fmt.pix.height;
    m_width = videodev.cap_width;
    m_height = videodev.cap_height;

    return 0;
  }

  int Camera::startCapture()
  {
    int a, ret;

    /* Start Streaming. on capture device */
    a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(videodev.fd, VIDIOC_STREAMON, &a);
    if (ret < 0) {
      log("capture VIDIOC_STREAMON error fd=x"); //FIXME: << videodev.fd;
      return ret;
    }
    log("Stream on...");

    return 0;
  }

  int Camera::captureFrame(CaptureCallback cb)
  {
    int ret;
    struct v4l2_buffer buf;

    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    ret = ioctl(videodev.fd, VIDIOC_DQBUF, &buf);
    if(ret < 0) {
      log("Cap VIDIOC_DQBUF");
      return ret;
    }

    /* TODO:
    if (frame_count++ % frame_devisor == 0)
        updateTexture((uchar *)videodev.buff_info[buf.index].start, videodev.cap_width, videodev.cap_height);
    */

    //std::cout << (frame_count++ % frame_devisor) << std::endl;
    /*
    if(frame_count++ % frame_devisor == 0) {
    }*/
    cb(&videodev, &buf);

    ret = ioctl(videodev.fd, VIDIOC_QBUF, &buf);
    if(ret < 0) {
      log("Cap VIDIOC_QBUF");
      return ret;
    }

    return 0;
  }

  int Camera::stopCapture()
  {
    int a, ret;

    log("Stream off!!");

    a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(videodev.fd, VIDIOC_STREAMOFF, &a);
    if (ret < 0) {
      log("VIDIOC_STREAMOFF");
      return ret;
    }

    return 0;
  }

  void Camera::closeDevice()
  {
    int i;
    struct buf_info *buff_info;

    /* Un-map the buffers */
    for (i = 0; i < CAPTURE_MAX_BUFFER; i++){
      buff_info = &videodev.buff_info[i];
      if (buff_info->start) {
        munmap(buff_info->start, buff_info->length);
        buff_info->start = NULL;
      }
    }

    if (videodev.fd >= 0) {
      close(videodev.fd);
      videodev.fd = -1;
    }
  }

  Frame Camera::getFrame() {

    Frame frame;

    /*captureFrame([this, frame](v4l2_buffer* buf) {
      formatFrame((uchar*)videodev.buff_info[buf->index].start, frame.data, m_width, m_height, m_stride);
    });*/

    //updateTexture((uchar *)videodev.buff_info[buf.index].start, videodev.cap_width, videodev.cap_height);
    //Frame frame;
    //formatFrame((uchar*)videodev.buff_info[buf.index].data, frame.data, m_width, m_height, m_stride);

    return frame;
  }

}};
