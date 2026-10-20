#include "motor_driver.h"

void motorSetup() {
        pinMode(REVERSE, OUTPUT);
        pinMode(FORWARD, OUTPUT);
        pinMode(REVERSE_2, OUTPUT);
        pinMode(FORWARD_2, OUTPUT);
}


void drive_single_forward(uint8_t motor, uint8_t pwm) {
  
        if (motor == 0) {
                digitalWrite(REVERSE, LOW);
                analogWrite(FORWARD, pwm);
        } else {
                digitalWrite(REVERSE_2, LOW);
                analogWrite(FORWARD_2, pwm);
        }
        delay(MOTOR_DELAY);

}

void drive_single_reverse(uint8_t motor, uint8_t pwm) {
        if (motor == 0) {
                digitalWrite(REVERSE, LOW);
                analogWrite(FORWARD, pwm);
        } else {
                digitalWrite(REVERSE_2, LOW);
                analogWrite(FORWARD_2, pwm);
        }
        delay(MOTOR_DELAY);
}

void drive_forward(uint8_t pwm) {
        drive_single_forward(0, pwm);
        drive_single_forward(1, pwm);
}

void drive_reverse(uint8_t pwm) {
        drive_single_reverse(0, pwm);
        drive_single_reverse(1, pwm);
}

void stop_motors() {
        digitalWrite(FORWARD, LOW);
        digitalWrite(REVERSE, LOW);
        digitalWrite(FORWARD_2, LOW);
        digitalWrite(REVERSE_2, LOW);
}

void turn_left(uint8_t pwm) {
        drive_single_forward(1, pwm);
        drive_single_forward(0, 0);
}

void turn_right(uint8_t pwm) {
        drive_single_forward(0, pwm);
        drive_single_forward(1, 0);
}
