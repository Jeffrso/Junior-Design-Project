#include <Arduino.h>
#include "state_machine.h"
#include "websocket_client.h"
#include "sevenSeg.h"

void setup() {
  Serial.begin(115200);
  sevenseg_setup();
  if (!TEST_MODE) {
    websocketSetup();     // test mode doesn't need WiFi
  }
}

void loop() {
  if (!TEST_MODE) {
    websocketLoop();      // check for new pings from the server
  }
  stateMachineUpdate();   // each state calls externalSignal() to see if a ping came in
}
