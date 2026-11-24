#include <Arduino.h>
#include "state_machine.h"
#include "motion_states.h"
#include "web.h"
#include "distance.h"
#include "motor_driver.h"
#include "distancedemo.h"
#include "color_sense.h"
#include "lane_follow.h"
#include "team_states.h"
#include "battery.h"


void setup() {
  Serial.begin(115200);
  // unsigned long serialWaitStart = millis();
  // while (!Serial && millis() - serialWaitStart < 3000) {
    delay(3000); // wait up to 3 s for serial monitor so startup prints aren't lost
  // }
  Serial.println("\n=== boot ===");
  // motorSetup();
  colorSetup(); // color sensor LEDs off before IR baseline is taken
  // irSensorSetup();
  // webSocket_ini(); 
  // battery_init();

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
  //communication with teams
  // webSocket_loop();
  // teamUpdate(); 
  Serial.print("STARt\n");
    Colors color_reads[2] = {};
    colorReadAll(color_reads);
      const char* sensorNames[3] = {"Left", "Right"};
    for (int c = 0; c < 2; c++) {
      Serial.print(sensorNames[c]);
      Serial.print(": ");
      Serial.println(colorName(color_reads[c]));
    }
  //battery reading
  check_low_battery();
 
}