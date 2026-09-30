#include "motor_driver.h"

void drive_forward(int pwm) {
        digitalWrite(REVERSE, LOW);
        analogWrite(FORWARD, pwm);\
        delay(500);
}

void drive_reverse(int pwm) {
        digitalWrite(FORWARD, LOW);
        analogWrite(REVERSE, pwm);
         delay(500);
}