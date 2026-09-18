// State Machine!!!
#include <Arduino.h>

typedef enum {
    STATE_IDLE,
    STATE_INITIALIZE,
    STATE_DRIVE,
    STATE_FIND_LANE,
    STATE_LANE_FOLLOW,
    STATE_DRIVE_WALL_TURN,
    STATE_TURN,
    STATE_RETURN_HOME
} RobotState;

// For global state whatev
RobotState currentState = STATE_IDLE;

//function stuff
bool externalSignal();
void stateMachineUpdate();

// NEED SOME INITIALIZATION STUFF HERE

bool externalSignal(){

}

void stateMachineUpdate() {
    switch (currentState) {
        case STATE_IDLE:
            if(externalSignal()) {
                Serial.print("idle to drive wall turn");
                currentState = STATE_DRIVE_WALL_TURN;
            }


            break;
        case STATE_INITIALIZE:
            if(externalSignal()) {
                Serial.print("init");
                currentState = STATE_IDLE;
            }
            break;
        // COULD REMOVE THEDRIVE STATE AND JUST HAVE RETURN HOME?????????
        case STATE_DRIVE:
            if(externalSignal()) {
                Serial.print("drive to");
                currentState = STATE_DRIVE_WALL_TURN;
            }
            break;
        case STATE_FIND_LANE:
            if(externalSignal()) {
                Serial.print("find lane to follow lane");
                currentState = STATE_LANE_FOLLOW;
            }
            break;
        case STATE_LANE_FOLLOW:
            if(externalSignal()) {
                Serial.print("lane follow to turn");
                currentState = STATE_TURN;
            }
            break;
        case STATE_DRIVE_WALL_TURN:
            if(externalSignal()) {
                Serial.print("drive wall turn to find lane");
                currentState = STATE_FIND_LANE;
            }

            break;
        case STATE_TURN:
            if(externalSignal()) {
                Serial.print("turn to drive");
                currentState = STATE_DRIVE;
            }

            break;
        case STATE_RETURN_HOME:
            if(externalSignal()) {
                Serial.print("return home to idle");
                currentState = STATE_IDLE;
            }

            break;
        default:
            currentState = STATE_IDLE;
            break;

    }
}