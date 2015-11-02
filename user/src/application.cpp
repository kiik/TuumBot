#include "application.hpp"
#include <iostream>

using namespace std;
using namespace rtx;

int main() {
  printf("main(): Default user application.\n");

  // Initialize hardware
  rtx::hal::setup();

  // Initialize system modules
  Visioning::setup();
  Localization::setup();

  bool running = true;
  while(running) {
    rtx::hal::process();

    Visioning::process();
    Localization::process();
  }
  return 0;
}
