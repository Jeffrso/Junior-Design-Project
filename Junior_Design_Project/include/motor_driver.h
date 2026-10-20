// Motor driver: 2 motors
#pragma once
#include <Arduino.h>

#define FORWARD 18 // motor 0
#define REVERSE 8 // motor 0
#define FORWARD_2 3 // motor 1
#define REVERSE_2 46 // motor 1

// true = motor bench test (no WiFi / no state machine, both motors forward at 200)
const bool MOTOR_TEST_MODE = false;

void motorSetup(); // call once in setup(): makes 4 motor pins outputs

// if motor is 0, then first motor runs
// if motor is 1, then second motor runs
void drive_forward(uint8_t motor, int pwm);
void drive_reverse(uint8_t motor, int pwm);