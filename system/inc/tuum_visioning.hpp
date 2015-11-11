/** @file tuum_visioning.hpp
 *  Visioning interface declaration.
 *
 *  @authors Ants-Oskar Mäesalu
 *  @authors Meelik Kiik
 *  @version 0.1
 *  @date 11. November 2015
 */

#ifndef RTX_VISIONING_H
#define RTX_VISIONING_H

#include <vector>
#include "__future__.hpp"

#include "rtxhal.hpp"
#include "rtxent.hpp"
#include "rtxvision.h"

using namespace vis;

namespace rtx { namespace Visioning {

  typedef std::vector<Feature*> FeatureSet;
  typedef std::vector<Ball*> BallSet;
  typedef std::vector<Robot*> RobotSet;
  typedef std::vector<Goal*> GoalSet;
  typedef std::vector<Robot*> RobotSet;

  extern std::string filter;

  extern FeatureSet features;
  extern BallSet balls;
  extern RobotSet robots;
  extern GoalSet goals;
  extern RobotSet robots;

  void setup();
  void process();

  void readFilterFromFile(const std::string&);

  void featureDetection(const Frame&);
  void ballDetection(const Frame&);
  void goalDetection(const Frame&);
  void robotDetection(const Frame&);

}}

#endif // RTX_VISIONING_H
