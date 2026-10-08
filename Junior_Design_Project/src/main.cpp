#include <Arduino.h>
#include "state_machine.h"
#include "web.h"
#include "sevenSeg.h"

void wsEchoTestLoop(); // TEMP: websocket echo test, delete after testing

void setup() {
  Serial.begin(115200);
  sevenseg_setup();
  if (!TEST_MODE) {
    webSocket_ini(); // test mode doesn't need WiFi
  }
}

void loop() {
  if (!TEST_MODE) {
    webSocket_loop(); // check for new pings from server
    wsEchoTestLoop(); // TEMP: websocket echo test, delete after testing
  }
  stateMachineUpdate(); // each state calls externalSignal() to see if a ping came in
}
