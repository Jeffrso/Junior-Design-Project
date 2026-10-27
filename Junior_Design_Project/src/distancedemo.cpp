#include "distance.h"
#include <Arduino.h>
#include "motor_driver.h"
#include "distancedemo.h"

typedef enum {
    STOP,
    D_REVERSE,
    D_FORWARD
} State;

// start stopped
static State current = D_FORWARD;
static unsigned long reverseStartTime = 0;
const unsigned long REVERSE_DURATION_MS = 1000; // Back up for 1 second

static void setStop() {
    drive_forward(0, 0);
    drive_forward(1, 0);
}

void updateState() {
    collisionUpdate();

    if (obstacleAhead()) {
        if(current != D_REVERSE) {
          current = STOP;
        }
    }

    switch (current) {
    case STOP:
    // motors to 0
    setStop();
    delay(500);
    // maybe a delay here
    reverseStartTime = millis();
    current = D_REVERSE;

    break;
    case D_FORWARD:
    drive_forward(1, 100);
    drive_forward(0, 100);
        if (collisionUpdate()) {
            current = STOP;
        }

    break;
    case D_REVERSE:
    drive_reverse(1, 100);
    drive_reverse(0, 100);

    if (millis() - reverseStartTime >= REVERSE_DURATION_MS) {
                // check if the path is clear now
                if (!obstacleAhead()) {
                    current = D_FORWARD; // Safe to resume normal driving!
                } else {
                    // If still blocked, reset the timer to back up more
                    reverseStartTime = millis(); 
                }
        }
        break;
    }
}
