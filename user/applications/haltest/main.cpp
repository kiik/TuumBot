#include "application.hpp"

#include <iostream>
#include "MotorDriver.hpp"
#include "MotorControl.hpp"
#include "rtxhal.hpp"










int main(){
  printf("haltest\n");
  //const char* dev = "/dev/ttyUSB0";
  //rtx::MotorControl* test = new rtx::MotorControl(dev, 115200);
  //test->forward(19);
  //test->turn(180);
  rtx::hal::setup();
  rtx::hal::hw.getMotorControl()->testSequence();
}