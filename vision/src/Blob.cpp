/**
 * @file Blob.cpp
 * Blob seen in the camera frame.
 *
 * @authors Ants-Oskar Mäesalu
 * @version 0.1
 */

#include "cameraConstants.hpp"

#include "Blob.hpp"

#include <algorithm>
#include <iostream> // TODO: Remove


namespace rtx {

  Blob::Blob(const Blob &other):
    position{new Point2D(*(other.getPosition()))},
    minX{other.getMinX()},
    maxX{other.getMaxX()},
    minY{other.getMinY()},
    maxY{other.getMaxY()},
    numberOfPoints{other.getNumberOfPoints()},
    color{other.getColor()}
  {
    // Nothing to do here
  }

  Blob::Blob(const std::vector<Point2D*> &points, const Color &color) {
    this->color = color;
    minX = CAMERA_WIDTH, minY = CAMERA_HEIGHT;
    maxX = 0, maxY = 0;
    unsigned int xSum = 0, ySum = 0;
    numberOfPoints = 0;
    for (std::vector<Point2D*>::const_iterator i = points.begin(); i != points.end(); ++i) {
      numberOfPoints++;
      xSum += (*i)->getX();
      ySum += (*i)->getY();
      if ((*i)->getX() < minX) {
        minX = (*i)->getX();
      }
      if ((*i)->getX() > maxX) {
        maxX = (*i)->getX();
      }
      if ((*i)->getY() < minY) {
        minY = (*i)->getY();
      }
      if ((*i)->getY() > maxY) {
        maxY = (*i)->getY();
      }
    }
    position = new Point2D(xSum / numberOfPoints, ySum / numberOfPoints);
  }

  Blob::Blob(const std::vector<std::pair<unsigned int, unsigned int>> &points, const Color &color) {
    this->color = color;
    unsigned int minX = CAMERA_WIDTH - 1, minY = CAMERA_HEIGHT - 1;
    unsigned int maxX = 0, maxY = 0;
    unsigned int xSum = 0, ySum = 0;
    numberOfPoints = 0;
    for (std::vector<std::pair<unsigned int, unsigned int>>::const_iterator i = points.begin(); i != points.end(); ++i) {
      numberOfPoints++;
      xSum += i->first;
      ySum += i->second;
      if (i->first < minX) {
        minX = i->first;
      }
      if (i->first > maxX) {
        maxX = i->first;
      }
      if (i->second < minY) {
        minY = i->second;
      }
      if (i->second > maxY) {
        maxY = i->second;
      }
    }
    position = new Point2D(xSum / numberOfPoints, ySum / numberOfPoints);
  }

  Blob::~Blob() {
    // TODO
  }

  Point2D* Blob::getPosition() const {
    return position;
  }

  unsigned int Blob::getWidth() const {
    return maxX - minX + 1;
  }

  unsigned int Blob::getHeight() const {
    return maxY - minY + 1;
  }

  unsigned int Blob::getMinX() const {
    return minX;
  }

  unsigned int Blob::getMaxX() const {
    return maxX;
  }

  unsigned int Blob::getMinY() const {
    return minY;
  }

  unsigned int Blob::getMaxY() const {
    return maxY;
  }

  unsigned int Blob::getNumberOfPoints() const {
    return numberOfPoints;
  }

  Color Blob::getColor() const {
    return color;
  }

  unsigned int Blob::getBoxArea() const {
    return getWidth() * getHeight();
  }

  double Blob::getDensity() const {
    return 1.0 * numberOfPoints / getBoxArea();
  }

  bool Blob::overlaps(const Blob &other) const {
    return color == other.getColor() && minX < other.getMaxX() && maxX > other.getMinX() && minY < other.getMaxY() && maxY > other.getMinY();
  }

  void Blob::join(const Blob &other) {
    // Define new box area
    minX = std::min(minX, other.getMinX());
    maxX = std::max(maxX, other.getMaxX());
    minY = std::min(minY, other.getMinY());
    maxY = std::max(maxY, other.getMaxY());
    // TODO: Add points
    numberOfPoints += other.getNumberOfPoints();
    // Calculate new position // TODO: Calculate based on points
    position->setX((position->getX() + other.getPosition()->getX()) / 2);
    position->setY((position->getY() + other.getPosition()->getY()) / 2);
  }

};
