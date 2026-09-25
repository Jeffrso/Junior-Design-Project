// State Machine!!!
#include <Arduino.h>
#include "state_machine.h"
#include "websocket_client.h"

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
void ledOn(int number);

// NEED SOME INITIALIZATION STUFF HERE

// True once for each ping from the websocket server, so each ping moves us to the next state
bool externalSignal(){
    return websocketPingReceived();
}

void ledOn(int number){

    if(number == 1) {

    } else if(number == 2) {

    } else if(number == 3) {

    } else if(number == 4) {

    } else if(number == 5) {

    } else if(number == 6) {

    } else if(number == 7) {

    } else if(number == 8) {

    } else {
        // base case, maybe just have an on LED
    }

}

void stateMachineUpdate() {
    switch (currentState) {
        case STATE_IDLE:
        ledOn(1);

            if(externalSignal()) {
                Serial.println("idle to drive wall turn");
                currentState = STATE_DRIVE_WALL_TURN;
            }


            break;
        case STATE_INITIALIZE:
        ledOn(2);

            if(externalSignal()) {
                Serial.println("init");
                currentState = STATE_IDLE;
            }
            break;
        // COULD REMOVE THEDRIVE STATE AND JUST HAVE RETURN HOME?????????
        case STATE_DRIVE:
        ledOn(3);

            if(externalSignal()) {
                Serial.println("drive to return home");
                currentState = STATE_RETURN_HOME;
            }
            break;
        case STATE_FIND_LANE:
        ledOn(4);

            if(externalSignal()) {
                Serial.println("find lane to follow lane");
                currentState = STATE_LANE_FOLLOW;
            }
            break;
        case STATE_LANE_FOLLOW:
        ledOn(5);

            if(externalSignal()) {
                Serial.println("lane follow to turn");
                currentState = STATE_TURN;
            }
            break;
        case STATE_DRIVE_WALL_TURN:
        ledOn(6);

            if(externalSignal()) {
                Serial.println("drive wall turn to find lane");
                currentState = STATE_FIND_LANE;
            }

            break;
        case STATE_TURN:
        ledOn(7);

            if(externalSignal()) {
                Serial.println("turn to drive");
                currentState = STATE_DRIVE;
            }

            break;
        case STATE_RETURN_HOME:
        ledOn(8);

            if(externalSignal()) {
                Serial.println("return home to idle");
                currentState = STATE_IDLE;
            }

            break;
        default:
            currentState = STATE_IDLE;
            break;

    }
}