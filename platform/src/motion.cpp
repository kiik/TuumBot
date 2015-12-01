
#include "motion.hpp"


namespace rtx { namespace Motion {

  const ValueLevels<int> VLS_DIST = {5000, 850, 700, 350, 200};
  const ValueLevels<double> VLS_ANGLE = {2*3.14, 0.38, 0.27, 0.20, 0.10};

  //TODO: Movement gears
  const Gears GRS_MOV = {{110, 275}, {80, 175}, {40, 100}, {35, 50}, {30, 50}};
  const Gears GRS_ROT = {{0, 0.3}, {1, 0.30}, {10, 0.20}, {12, 0.05}, {20, 0.4}};

}}