/** @file Transform.cpp
 *  Object positional transform state class.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 *  @date 19. November 2015
 */

#include "Transform.hpp"

namespace rtx {

  Transform::Transform() {
    pos.x = 0; pos.y = 0;
    o = 0;
  }

  Transform::Transform(const int, const int) {

  }

  Transform::Transform(const Vec2i*) {

  }

  Transform::Transform(const int, const int, const double) {

  }

  Transform::Transform(const Vec2i*, const double) {

  }

  void Transform::setPosition(const Vec2i nPos) {
    pos = nPos;
  }

  Vec2i Transform::getPosition() {
    return pos;
  }

  int Transform::getX() {
    return pos.x;
  }

  int Transform::getY() {
    return pos.y;
  }

}