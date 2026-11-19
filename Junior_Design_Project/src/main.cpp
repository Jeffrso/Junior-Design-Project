#include <Arduino.h>
#include "state_machine.h"
#include "motion_states.h"
#include "web.h"
#include "distance.h"
#include "motor_driver.h"
#include "distancedemo.h"
#include "color_sense.h"
#include "lane_follow.h"

void setup() {
  Serial.begin(115200);
  unsigned long serialWaitStart = millis();
  while (!Serial && millis() - serialWaitStart < 3000) {
    delay(10); // wait up to 3 s for serial monitor so startup prints aren't lost
  }
  Serial.println("\n=== boot ===");
  motorSetup();
  colorSetup(); // color sensor LEDs off before IR baseline is taken
  irSensorSetup();
  webSocket_ini(); 

  // if (COLOR_TEST_MODE || LANE_TEST_MODE || COLLISION_TEST_MODE || MOTOR_TEST_MODE) {
  //   return; // bench tests don't need WiFi
  // }

  // if (MOTION_MODE) {
  //   motionSetup(); // bot motions milestone (motion_states.cpp)
  // } else {
  //   stateMachineSetup(); // state machine milestone (state_machine.cpp)
  // }

  // if (!TEST_MODE) {
  // webSocket_ini(); // test mode doesn't need WiFi
  // }
}

void loop() {

  webSocket_loop(); 

  // collisionUpdate(); // keep obstacle decision up to date (obstacleAhead())
  if (MOTION_MODE) {
    motionUpdate(); // Enter = next motion, websocket MOVE command = jump to that motion
  } else {
    stateMachineUpdate(); // Enter = next state, websocket STATE command = jump to that state
  }
}
