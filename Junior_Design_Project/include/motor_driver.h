// Motor driver: 2 motors
#pragma once
#include <Arduino.h>

#define FORWARD 8 // motor 0
#define REVERSE 18 // motor 0
#define FORWARD_2 46 // motor 1
#define REVERSE_2 3 // motor 1

// true = motor bench test (no WiFi / no state machine, both motors forward at 200)
const bool MOTOR_TEST_MODE = false;

// Calibration Related Things
const uint8_t LEFT_MOTOR = 1; // SWAP IF TURNS GO WRONG WAY
const uint8_t RIGHT_MOTOR = 0;

// Motor mismatch: lower faster motor's trim until bot drives straight (keep both <= 1.0)
const float LEFT_TRIM = 1.00;
const float RIGHT_TRIM = 1.00;

const int DRIVE_PWM = 180; // forward/backward speed (0-255)
const int PIVOT_PWM = 150; // pivot speed (0-255)
const int TURN_OUTER_PWM = 255; // faster (outside) wheel in a turn: full speed
const int TURN_INNER_PWM = 100; // slower (inside) wheel: LOWER = tighter turn, higher = wider
const float TURN_RADIUS_CM = 30.0; // turn radius

void motorSetup(); // call once in setup(): makes 4 motor pins outputs, motors stopped

// Both wheels at once: -255...255 each (+ is forward, - is backward, 0 = stop)
void drive(int left, int right);

// if motor is 0, then first motor runs
// if motor is 1, then second motor runs
void drive_forward(uint8_t motor, int pwm);
void drive_reverse(uint8_t motor, int pwm);