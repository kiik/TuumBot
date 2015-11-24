/** @file tuum_navigation.hpp
 *  Navigation system interface.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 */

#ifndef RTX_NAVIGATION_H
#define RTX_NAVIGATION_h

#include "rtxmath.hpp"

#include "Ball.hpp"
#include "Goal.hpp"

namespace rtx { namespace Navigation {

  Transform calcBallPickupPos(Transform*);
  Vec2i calcGoalShootPos(Transform*);

  Goal* getOpposingGoal();
  Ball* getNearestBall();

}}

#endif // RTX_NAVIGATION_H
