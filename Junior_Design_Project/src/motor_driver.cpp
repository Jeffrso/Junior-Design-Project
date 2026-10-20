#include "motor_driver.h"

void motorSetup() {
    pinMode(REVERSE, OUTPUT);
    pinMode(FORWARD, OUTPUT);
    pinMode(REVERSE_2, OUTPUT);
    pinMode(FORWARD_2, OUTPUT);
}

void drive_forward(uint8_t motor, int pwm) {
    if (motor == 0) {
        digitalWrite(REVERSE, LOW);
        analogWrite(FORWARD, pwm);
    } else {
        digitalWrite(REVERSE_2, LOW);
        analogWrite(FORWARD_2, pwm);
    }
    delay(50);
}

void drive_reverse(uint8_t motor, int pwm) {
    if (motor == 0) {
        digitalWrite(FORWARD, LOW);
        analogWrite(REVERSE, pwm);
    } else {
        digitalWrite(FORWARD_2, LOW);
        analogWrite(REVERSE_2, pwm);
    }
    delay(50);
}
