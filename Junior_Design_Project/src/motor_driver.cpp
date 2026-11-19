#include "motor_driver.h"

// PWM is set up ONCE in motorSetup(). After that, ledcWrite() only changes duty cycle.
const uint32_t PWM_FREQ = 1000; // Hz (same as analogWrite default)
const uint8_t PWM_BITS = 8; // duty 0-255

// One PWM channel per motor pin
const uint8_t CH_FORWARD = 0;
const uint8_t CH_REVERSE = 1;
const uint8_t CH_FORWARD_2 = 2;
const uint8_t CH_REVERSE_2 = 3;

static void pwmSetup(uint8_t pin, uint8_t channel) {
    ledcSetup(channel, PWM_FREQ, PWM_BITS);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, 0); // start stopped
}

void motorSetup() {
    pwmSetup(FORWARD, CH_FORWARD);
    pwmSetup(REVERSE, CH_REVERSE);
    pwmSetup(FORWARD_2, CH_FORWARD_2);
    pwmSetup(REVERSE_2, CH_REVERSE_2);
}

// No delay: drive() sets both motors back to back so they start and stop together
void drive_forward(uint8_t motor, int pwm) {
    if (motor == 0) {
        ledcWrite(CH_REVERSE, 0);
        ledcWrite(CH_FORWARD, pwm);
    } else {
        ledcWrite(CH_REVERSE_2, 0);
        ledcWrite(CH_FORWARD_2, pwm);
    }
}

void drive_reverse(uint8_t motor, int pwm) {
    if (motor == 0) {
        ledcWrite(CH_FORWARD, 0);
        ledcWrite(CH_REVERSE, pwm);
    } else {
        ledcWrite(CH_FORWARD_2, 0);
        ledcWrite(CH_REVERSE_2, pwm);
    }
}

// One motor: + is forward, - is backward, 0 is stopped
// Both pins low = L293D brakes the motor
static void setMotor(uint8_t motor, int pwm) {
    pwm = constrain(pwm, -255, 255);
    if (pwm >= 0) {
        drive_forward(motor, pwm);
    } else {
        drive_reverse(motor, -pwm);
    }
}

void drive(int left, int right) {
    setMotor(LEFT_MOTOR, (int)(left * LEFT_TRIM));
    setMotor(RIGHT_MOTOR, (int)(right * RIGHT_TRIM));
}
