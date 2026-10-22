#if 0

#include "distance.h"

typedef enum {
    STOP,
    REVERSE,
    FOWARD
} State;

// start stopped
State current = STOP;

void updateState() {
    if(obstacleAhead()) {
        if(current =! REVERSE) {
          current = STOP;
        }
    }
}

switch (current) {
  case STOP:
  // motors to 0
  // maybe a delay here
  current = REVERSE;

  break;
  case FOWARD:
      if(collisionUpdate()) {
        current = STOP;
      }

  break;
  case REVERSE:
  // motors to reverse

  break;
}

static unsigned long reverseTimer = 0;
const unsigned long REVERSE_DURATION_MS = 800; // How long to back up (800ms)

void stateMachineUpdate() {
  // 1. SAFETY OVERRIDE: Check for obstacle regardless of current state
  if (obstacleAhead()) {
    // If we weren't already reversing, force an immediate stop & trigger reverse
    if (currentRobotState != STATE_OBSTACLE_REVERSE) {
      currentRobotState = STATE_OBSTACLE_STOP;
    }
  }

  // 2. STATE MACHINE LOGIC
  switch (currentRobotState) {
    
    case STATE_NORMAL:
      // Your normal robot movement / web socket control goes here
      // e.g., moveForward();
      break;

    case STATE_OBSTACLE_STOP:
      // Step A: Stop the motors instantly
      stopMotors(); 
      
      // Brief pause or transition straight into reversing
      reverseTimer = millis();
      currentRobotState = STATE_OBSTACLE_REVERSE;
      break;

    case STATE_OBSTACLE_REVERSE:
      // Step B: Drive backward
      moveBackward();

      // Step C: Check if we have reversed long enough AND the path is clear now
      if (millis() - reverseTimer >= REVERSE_DURATION_MS) {
        // Only resume normal movement if the obstacle is actually gone
        if (!obstacleAhead()) {
          stopMotors();
          currentRobotState = STATE_NORMAL;
        } else {
          // If still blocked, reset the timer to back up a bit more
          reverseTimer = millis(); 
        }
      }
      break;
  }
}

#endif