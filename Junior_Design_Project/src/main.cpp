// #include <Arduino.h>
// #include "state_machine.h"
// #include "motion_states.h"
// #include "web.h"
// #include "distance.h"
// #include "motor_driver.h"
// #include "distancedemo.h"
// #include "color_sense.h"
// #include "lane_follow.h"
// #include "team_states.h"
// #include "battery.h"

// void setup() {
//   Serial.begin(115200);
//   // unsigned long serialWaitStart = millis();
//   // while (!Serial && millis() - serialWaitStart < 3000) {
//   //   delay(3000); // wait up to 3 s for serial monitor so startup prints aren't lost
//   // }
//   Serial.println("\n=== boot ===");
//   // motorSetup();
//   // colorSetup(); // color sensor LEDs off before IR baseline is taken
//   // irSensorSetup();
//   // webSocket_ini(); 
//   // battery_init();

//   // if (MOTION_MODE) {
//   //   motionSetup(); // bot motions milestone (motion_states.cpp)
//   // } else {
//   //   stateMachineSetup(); // state machine milestone (state_machine.cpp)
//   // }

//   // if (!TEST_MODE) {
//   // webSocket_ini(); // test mode doesn't need WiFi
//   // }
// }

// void loop() {
//   //communication with teams
//   // webSocket_loop();
//   // teamUpdate();
//   // while (1) {
//   //   Serial.print("working");
//   // }
//   // Serial.print("STARt");
//   // Colors color_reads[4] = {};
//   // colorReadAll(color_reads);
//   // const char* sensorNames[3] = {"Left", "Middle", "Right"};
//   // for (int c = 0; c < 3; c++) {
//   //   Serial.print(sensorNames[c]);
//   //   Serial.print(": ");
//   //   Serial.println(colorName(color_reads[c]));
//   // }
//   //battery reading
//   // check_low_battery();
// }
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

  if (COLOR_TEST_MODE || LANE_TEST_MODE || COLLISION_TEST_MODE || MOTOR_TEST_MODE) {
    return; // bench tests don't need WiFi
  }
}

void loop() {
  if (COLOR_TEST_MODE) {
    colorTestLoop(); // prints all 3 color sensors, calibration commands
    return;
  }
  if (LANE_TEST_MODE) {
    laneTestLoop(); // y / r / b + Enter = follow that lane, Enter = stop
    return;
  }
  if (STATE_TEST_MODE) {
    updateState();
    collisionTestLoop(); // Runs entire state machine loop (reads sensor + runs motors)
    return;
  }
  if (COLLISION_TEST_MODE) {
    collisionTestLoop(); // prints sensor data
    return;
  }
  if (MOTOR_TEST_MODE) {
    drive_forward(0, 200); // both motors forward
    drive_forward(1, 200);
    delay(500);
    return;
  }
  if (!TEST_MODE) {
    webSocket_loop(); // check for new pings from server
  }

  collisionUpdate(); // keep obstacle decision up to date (obstacleAhead())
}
