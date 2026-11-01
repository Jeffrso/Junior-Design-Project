#include <Arduino.h>
#include "state_machine.h"
#include "motion_states.h"
#include "web.h"
#include "distance.h"
#include "motor_driver.h"
#include "distancedemo.h"
#include "color_sense.h"

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(COLOR_SENSOR, INPUT);
  pinMode(BATTERY_IN, INPUT);


}


void loop() {

  float voltage_value = 0;

  while(1) {

    uint8_t rby_pins[3] = {RED_PIN, GREEN_PIN, BLUE_PIN};
    Colors color = read_color(COLOR_SENSOR, rby_pins);

      Serial.print("DETECTED: ");
      if (color == RED_t) {
        Serial.println("RED");
      } else if (color == BLUE_t) {
         Serial.println("BLUE");
      } else if (color == YELLOW_t) {
          Serial.println("YELLOW");
      } else {
          Serial.println("BLACK");
      }

    delay(1000);

  }
}
