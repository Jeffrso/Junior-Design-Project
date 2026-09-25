#include <Arduino.h>
#include "state_machine.h"
#include "websocket_client.h"

void setup() {
  Serial.begin(115200);
  websocketSetup();
}

void loop() {
  websocketLoop();        // check for new pings from the server
  stateMachineUpdate();   // each state calls externalSignal() to see if a ping came in
}
