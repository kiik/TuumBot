/** @file tuum_motion.cpp
 *  Motion system implementation.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 *  @date 1. November 2015
 */

#include <iostream>

#include "rtxhal.hpp"
#include "MotorControl.hpp"

#include "tuum_localization.hpp"
#include "tuum_motion.hpp"


//TODO: Implement state machine & controllers
//TODO: Refactor MotionData structure to separate source
namespace rtx { namespace Motion {

  MotionType motionType;

  Transform motionGoal; // (x, y, orient)
  bool motionInProgress;
  bool targetAchieved;

  struct MotionData {
    int baseVelocity;
    Vec2f dV;
    double orientDelta;

    double getHeading() {
      //Vec2f _dV = dV;
      //_dV.rotate(-orientDelta);
      return atan2(dV.y, dV.x);
    }

    int getSpeed() {
      // TODO Transform this to real motor speed ( VelocityToSpeed(vel) )
      return (int)(baseVelocity * getVelocityFactor());
    }

    int getRotationSpeed() {
      // TODO: Transform orientVelocity into rotationSpeed ( RVelocityToSpeed(r_vel)  )
      double v = getOrientVelocity();
      return (int)v;
    }

    double getOrientVelocity() {
      double oD = fabs(orientDelta);
      int sign = 1;
      if(orientDelta < 0) sign = -1;

      if(oD <= 0.08) return 0;

      //TODO: is this correct?
      //if(oD > 3.14) oD = 3.14 - orientDelta;

      if(oD > 0.3) return sign*baseVelocity;

      double v = (oD - 0.08) * baseVelocity * sign / 0.22;

      if(fabs(v) < 10) {
        sign = 1;
        if(v < 0) sign = -1;
	return 10 * sign;
      }
      
      return v;
    }

    double getVelocityFactor() {
      double s = fabs(dV.x) + fabs(dV.y);

      if( s > 1) return 1;
      else if( s < -1 ) return -1;
      return s;
    }

    void setDirectionVector(double x, double y) {
      dV.x = x; dV.y = y;
      dV.normalizeTo(1.0);
    }

    void normalizeTo(double factor) {
      if(dV.x == 0.0 && dV.y == 0.0) return;
      double s = sqrt(pow(dV.x, 2) + pow(dV.y, 2)) / factor;
      dV.x /= s; dV.y /= s;
    }

  } motionData;

  bool motionActive = false;

  double targetDistanceCondition = 10;
  double targetOrientationCondition = 0.1;

  Timer motorCmdTimer;

  void setup() {
    motionType = MOT_NAIVE;
    motionInProgress = false;
    targetAchieved = false;

    motionData = {0, 0.0, 0.0, 0.0};

    motorCmdTimer.setPeriod(100);
    motorCmdTimer.start();

    printf("\033[1;32m");
    printf("[Motion::setup()]Ready.");
    printf("\033[0m\n");
  }

  void process() {
    hal::MotorControl* mco = hal::hw.getMotorControl();

    bool dirty = false;
    switch(motionType) {
      case MOT_SCAN:
        if(!motionInProgress && motionActive) {
          //printf("MOT_SCAN: init\n");
          motionData.baseVelocity = 0;
          motionData.setDirectionVector(0.0, 0.0);
          motionData.orientDelta = M_PI;

          motionInProgress = true;
          targetAchieved = false;
          dirty = true;
        }
        break;
      case MOT_NAIVE:
        // Correct orientation => motionData
        // then
        // Forward motion => motionData
        break;
      case MOT_CURVED:
        if(!motionInProgress && motionActive) {
          Transform* t = Localization::getTransform();

          motionData.baseVelocity = 25;
          motionData.orientDelta = motionGoal.o - t->o;
          motionData.setDirectionVector(motionGoal.getX() - t->getX(), motionGoal.getY() - t->getY());

          printf("MOT_CURVED motionData: vx=%g, vy=%g, do=%g\n", motionData.dV.x, motionData.dV.y, motionData.orientDelta);

          printf("MOT_CURVED: mag=%g, Vb=%i\n", motionData.dV.getMagnitude(), motionData.baseVelocity);

          printf("MOT_CURVED: orientVelocity=%g\n", motionData.getOrientVelocity());
          printf("MOT_CURVED: correctedHeading=%g\n", motionData.getHeading());

          targetAchieved = false;
          motionInProgress = true;
          dirty = true;
        }
        break;
      case MOT_STATIC:
        // Rotational motion => motionData
        break;
      case MOT_COMPLEX_CURVED:
        // Complex rotational motion => motionData
        break;
      default:
        motionData = {0, 0.0, 0.0};
        motionInProgress = false;
        dirty = true;
        break;
    }

    if(motionInProgress || dirty) {
      if(!isTargetAchieved()) {
	if(motorCmdTimer.isTime() || dirty) {
	  printf("[rtx::Motion]mco->omniDrive(%i, %g, %i)\n", motionData.getSpeed(), motionData.getHeading(), motionData.getRotationSpeed());
	  mco->OmniDrive(motionData.getSpeed(), motionData.getHeading(), motionData.getRotationSpeed());
	  motorCmdTimer.start();
	}
      } else {
	printf("[Motion]Target achieved.\n");
        stop();
	motionInProgress = false;
      }
    }

  }

  int setTarget(Transform target) {
    printf("[Motion::setTarget]%i, %i, %g\n", target.getX(), target.getY(), target.o);
    motionGoal = target;
    motionInProgress = false;
    targetAchieved = false;
  }

  void start() {
    motionActive = true;
  }

  void stop() {
    motionInProgress = false;
    hal::hw.getMotorControl()->OmniDrive(0, 0, 0);
  }

  void setSpeed(int v) {
    motionData.baseVelocity = v;
    // Recalculate velocity parameters?
  }

  void setBehaviour(MotionType mt) {
    stop();
    motionType = mt;
    motionActive = false;
  }

  double targetDistance() {
    return Localization::getTransform()->getPosition().distanceTo(motionGoal.getPosition());
  }

  double targetAngle() {
    return 0.0; // TODO: dot(Localization::getOrientation(), motionGoal);
  }

  double stateProbability(Transform* t1, Transform* t2) {
    double px = gaussian_probability(t1->getX(), 30, t2->getX());
    double py = gaussian_probability(t1->getY(), 30, t2->getY());
    double c  = 3.14/180;
    double po = gaussian_probability(t1->getOrientation()*c, 40, t2->getOrientation()*c);

    /*std::cout << "px=" << px
              << ", py=" << py
	      << ", po= " << po
	      << std::endl;
    */

    return (px + py + po) / 3;
  }

  bool isTargetAchieved() {
    if(!targetAchieved) {
      // (transform - target) <= uncertainty

      Transform* t = Localization::getTransform();
      double p = stateProbability(t, &motionGoal);

      //std::cout << "P = " << p << std::endl;

      if(p > 0.0115) targetAchieved = true;
    }

    return targetAchieved;
  }

  bool orientationAchieved() {
    return targetAngle() < targetOrientationCondition;
  }

}}
