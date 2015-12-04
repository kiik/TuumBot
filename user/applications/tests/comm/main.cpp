#include <cstdlib>
#include <iostream>
#include <string>

#include "application.hpp"

#include "rtx_ctl.hpp"

#include "rtxhal.hpp"

#include "tuum_communication.hpp"

using namespace std;
using namespace rtx;
using namespace hal;
using namespace comm;



int main(int argc, char* argv[]) {
  printf("Running communication tests...\n");
  rtx::init(argc, argv);

  hal::setup();

  bool running = true;

  TuumMessage tmsPass = TuumMessage::toAlly(TuumSignal::PASS);
  std::cout << "Sending pass message..." << std::endl;
  hal::hw.getRefListener()->sendTuumMessage(tmsPass);

  tmsPass = TuumMessage::toAlly(TuumSignal::PASS);
  std::cout << "Sending pass message..." << std::endl;
  hal::hw.getRefListener()->sendTuumMessage(tmsPass);

  tmsPass = TuumMessage::toAlly(TuumSignal::PASS);
  std::cout << "Sending pass message..." << std::endl;
  hal::hw.getRefListener()->sendTuumMessage(tmsPass);

  std::cout << "Waiting for response..." << std::endl;
  while(running) {
    hal::process();

    if(pollResponse(tmsPass.id)) {
      std::cout << "Response received" << std::endl;
      running = false;
    }
  }

  printf("Communication tests done.\n");
  return 0;
}
