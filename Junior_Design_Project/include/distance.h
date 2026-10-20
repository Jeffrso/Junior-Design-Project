// IR collision sensor.
//
// Wiring (runs off 3V3):
//   GPIO 17 -> 100 ohm -> IR LED long leg,  IR LED short leg -> GND
//   3V3 -> phototransistor short leg (collector),  long leg (emitter) -> GPIO 7  and  -> 10k -> GND
//   More IR light on phototransistor = higher voltage on GPIO 7
#pragma once
#include <Arduino.h>

const uint8_t IR_LED_PIN = 17;
const uint8_t IR_LED_PIN_2 = 16;
const uint8_t IR_SENSE_PIN = 7; // ADC1 pin

// true = bench test (no WiFi / no state machine, prints sensor 10 times a second)
const bool COLLISION_TEST_MODE = false;

void irSensorSetup(); // call once in setup() & also takes baseline
bool collisionUpdate(); // call every loop(): takes reading, returns true while an obstacle is too close
bool obstacleAhead(); // result of last collisionUpdate() (no new reading)
void collisionTestLoop(); // bench test loop used when COLLISION_TEST_MODE is true