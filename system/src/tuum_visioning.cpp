/** @file tuum_vision.cpp
 *  Visioning interface implementation.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 */

#include "rtxhal.hpp"
#include "tuum_visioning.hpp"

using namespace hal;

namespace rtx { namespace Visioning {

  extern FeatureSet features;
  extern BallSet balls;
  extern RobotSet robots;
  extern GoalSet goals;

  void setup() {
    CameraDevice* cam = hal::hw.getFrontCamera();
  }

  void process() {
    CameraDevice* cam = hal::hw.getFrontCamera();

    if(cam != nullptr) {
      featureDetection(cam);
      ballsDetection(cam);
      robotsDetection(cam);
      goalDetection(cam);
    }
  }

  void featureDetection(CameraDevice* cam) {

  }

  void ballsDetection(CameraDevice* cam) {

  }

  void robotsDetection(CameraDevice* cam) {

  }

  void goalDetection(CameraDevice* cam) {

  }

}}