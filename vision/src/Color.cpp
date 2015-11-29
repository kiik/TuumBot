/**
 * @file Color.hpp
 * Entity color definition.
 *
 * @authors Ants-Oskar Mäesalu
 * @version 0.1
 * @date 29 November 2015
 */

#include "Color.hpp"

#include "entityConstants.hpp"

#include "Perspective.hpp"


namespace rtx {

  Color intToColor(const unsigned int &value) {
    return static_cast<Color>(value);
  }

  void getRGB(const Color &color, unsigned int &r, unsigned int &g, unsigned int &b) {
    switch (color) {
      case BALL:
        r = 255;
        g = 128;
        b = 0;
        break;
      case BLUE_GOAL:
        r = 0;
        g = 0;
        b = 255;
        break;
      case YELLOW_GOAL:
        r = 255;
        g = 255;
        b = 0;
        break;
      case FIELD:
        r = 0;
        g = 255;
        b = 0;
        break;
      case ROBOT_YELLOW_BLUE:
        r = 102;
        g = 0;
        b = 0;
        break;
      case ROBOT_BLUE_YELLOW:
        r = 102;
        g = 0;
        b = 51;
        break;
      case WHITE_LINE:
      case CHECKERBOARD_WHITE:
        r = 255;
        g = 255;
        b = 255;
        break;
      case BLACK_LINE:
      case CHECKERBOARD_BLACK:
      default:
        r = 0;
        g = 0;
        b = 0;
        break;
    }
  }

  std::pair<double, double> getBlobExpectedRealSize(const Color &color) { // TODO: Change to mapping structure?
    switch (color) {
      case BALL:
        return std::pair<double, double>(BALL_RADIUS * 2, BALL_RADIUS * 2);
        break;
      case BLUE_GOAL:
      case YELLOW_GOAL:
        return std::pair<double, double>(GOAL_WIDTH, GOAL_HEIGHT);
        break;
      case FIELD:
        return std::pair<double, double>(10, 10); // TODO: Calibrate
        break;
      case ROBOT_YELLOW_BLUE:
      case ROBOT_BLUE_YELLOW:
        return std::pair<double, double>(ROBOT_MAXIMUM_DIAMETER, ROBOT_MARKER_WIDTH * 2); // TODO: Change when detecting arbitrary robot blobs
        break;
      case WHITE_LINE:
      case BLACK_LINE:
        return std::pair<double, double>(LINE_WIDTH, LINE_WIDTH);
        break;
      case CHECKERBOARD_WHITE:
      case CHECKERBOARD_BLACK:
      default:
        return std::pair<double, double>(0, 0);
        break;
    }
  }

  std::pair<unsigned int, unsigned int> getBlobExpectedVirtualSize(const Color &color, const std::pair<unsigned int, unsigned int> &position) {
    std::pair<double, double> realSize = getBlobExpectedRealSize(color);
    return realSize;
  }

}
