#include "motor_driver.h"

void drive_forward(int pwm) {
        digitalWrite(REVERSE, LOW);
        analogWrite(FORWARD, pwm);
}

void drive_reverse(int pwm) {
        digitalWrite(FORWARD, LOW);
        analogWrite(REVERSE, pwm);
}