#include <Arduino.h>
#include "state_machine.h"
#include "web.h"
#include "sevenSeg.h"
#include "distance.h"

void wsEchoTestLoop(); // TEMP: websocket echo test, delete after testing

void setup() {
  Serial.begin(115200);
  unsigned long serialWaitStart = millis();
  while (!Serial && millis() - serialWaitStart < 3000) {
    delay(10); // wait up to 3 s for the serial monitor so startup prints aren't lost
  }
  Serial.println("\n=== boot ===");
  //sevenseg_setup();
  irSensorSetup();

  if (COLLISION_TEST_MODE) {
    return; // collision bench test doesn't need WiFi
  }
  if (!TEST_MODE) {
    webSocket_ini(); // test mode doesn't need WiFi
  }
}

void loop() {
  if (COLLISION_TEST_MODE) {
    collisionTestLoop(); // prints sensor data. Type m = measure, b = new baseline
    return;
  }
  if (!TEST_MODE) {
    webSocket_loop(); // check for new pings from server
    wsEchoTestLoop(); // TEMP: websocket echo test, delete after testing
  }
  collisionUpdate(); // keep obstacle decision up to date (obstacleAhead())
  stateMachineUpdate(); // each state calls externalSignal() to see if a ping came in
}
