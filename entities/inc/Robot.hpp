/**
 * @file Robot.hpp
 * Robot class.
 *
 * @authors Ants-Oskar Mäesalu, Meelik Kiik
 * @version 0.1
 */

#ifndef RTX_ENTITIES_ROBOT_H
#define RTX_ENTITIES_ROBOT_H

#include "Circle.hpp"
#include "Point2D.hpp"

#include "entityConstants.hpp"
#include "Entity.hpp"


namespace rtx {

  class Robot : public Entity, public Circle {
  private:
    // TODO

  public:
    Robot();
    Robot(const Robot&);

    Robot(const Transform, const double& = ROBOT_MAXIMUM_DIAMETER / 2);
  };

};

#endif // RTX_ENTITIES_ROBOT_H
