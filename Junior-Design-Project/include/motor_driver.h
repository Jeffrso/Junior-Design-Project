#include <Arduino.h>
#define FORWARD 18
#define REVERSE 8
#define FORWARD_2 3
#define REVERSE_2 46
#define MOTOR_DELAY 50





void motorSetup();
void drive_single_forward(uint8_t motor, uint8_t pwm);
void drive_single_reverse(uint8_t motor, uint8_t pwm);
void drive_forward(uint8_t pwm);
void drive_reverse( uint8_t pwm);
void stop_motors(); 
void turn_left(uint8_t pwm);
void turn_right(uint8_t pwm);



