#include <Arduino.h>
#define FORWARD 18
#define REVERSE 8
#define FORWARD_2 3
#define REVERSE_2 46


// void drive_forward(int pwm);
// void drive_reverse(int pwm);

//if motor is 0 then first motor runs
// if motor is 1 then second motor runs
void drive_forward(uint8_t motor,int pwm);
void drive_reverse( uint8_t motor ,int pwm);
