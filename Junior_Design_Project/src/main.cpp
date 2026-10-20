#include <Arduino.h>
#include "state_machine.h"
#include "web.h"
#include "distance.h"

void setup() {
  Serial.begin(115200);
  unsigned long serialWaitStart = millis();
  while (!Serial && millis() - serialWaitStart < 3000) {
    delay(10); // wait up to 3 s for serial monitor so startup prints aren't lost
  }
  Serial.println("\n=== boot ===");
  irSensorSetup();

  if (COLLISION_TEST_MODE) {
    return; // collision bench test doesn't need WiFi
  }
  stateMachineSetup(); // turn on Enter-key interrupt
  if (!TEST_MODE) {
    webSocket_ini(); // test mode doesn't need WiFi
  }
}

void loop() {
  if (COLLISION_TEST_MODE) {
    collisionTestLoop(); // prints sensor data
    return;
  }
  if (!TEST_MODE) {
    webSocket_loop(); // check for new pings from server
  }
  collisionUpdate(); // keep obstacle decision up to date (obstacleAhead())
  stateMachineUpdate(); // Enter = next state, websocket command = jump to that state
}
