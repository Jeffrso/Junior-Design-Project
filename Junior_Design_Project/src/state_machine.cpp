// State Machine!!!
#include <Arduino.h>
#include "state_machine.h"
#include "websocket_client.h"
#include "sevenSeg.h"


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
    uint8_t segs = sevenseg_decode(number);

    // Assuming bit 6 is segment 'a' down to bit 0 for segment 'g'
    for (int i = 0; i < 7; i++) {
        bool bit_val = (segs >> (6 - i)) & 0x01;
        digitalWrite(SEG_PINS[i], bit_val);
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