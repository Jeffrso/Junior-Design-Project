// State Machine
#include <Arduino.h>
#include "state_machine.h"
#include "web.h"
#include "sevenSeg.h"

// In the order the robot goes through:
typedef enum {
    STATE_INITIALIZE,       // 1
    STATE_IDLE,             // 2
    STATE_DRIVE_WALL_TURN,  // 3
    STATE_FIND_LANE,        // 4
    STATE_LANE_FOLLOW,      // 5
    STATE_TURN,             // 6
    STATE_DRIVE,            // 7
    STATE_RETURN_HOME       // 8
} RobotState;

// For global state whatev
RobotState currentState = STATE_INITIALIZE;

//function stuff
bool externalSignal();
void stateMachineUpdate();
void ledOn(int number);

// NEED SOME INITIALIZATION STUFF HERE

// True once for each ping from the websocket server, so each ping moves us to the next state
bool externalSignal(){
    if (TEST_MODE) {
        // fake a ping every TEST_PING_MS so we can watch it cycle through the states
        static unsigned long lastPing = 0;
        if (millis() - lastPing >= TEST_PING_MS) {
            lastPing = millis();
            return true;
        }
        return false;
    }
    return webSocket_ping_received();
}

void ledOn(int number){
    uint8_t segs = sevenseg_decode(number);
    if (SEG_ACTIVE_LOW) {
        segs = ~segs;   // lookup table is 1 = on (common cathode), flip it for common anode
    }

    for (int i = 0; i < 7; i++) {
        bool bit_val = (segs >> (6 - i)) & 0x01;
        digitalWrite(SEG_PINS[i], bit_val);
    }
}

void stateMachineUpdate() {
    switch (currentState) {
        case STATE_INITIALIZE:
        ledOn(1);

            if(externalSignal()) {
                Serial.println("init to idle");
                currentState = STATE_IDLE;
            }
            break;
        case STATE_IDLE:
        ledOn(2);

            if(externalSignal()) {
                Serial.println("idle to drive wall turn");
                currentState = STATE_DRIVE_WALL_TURN;
            }

            break;
        case STATE_DRIVE_WALL_TURN:
        ledOn(3);

            if(externalSignal()) {
                Serial.println("drive wall turn to find lane");
                currentState = STATE_FIND_LANE;
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
        case STATE_TURN:
        ledOn(6);

            if(externalSignal()) {
                Serial.println("turn to drive");
                currentState = STATE_DRIVE;
            }

            break;
        // COULD REMOVE THEDRIVE STATE AND JUST HAVE RETURN HOME?????????
        case STATE_DRIVE:
        ledOn(7);

            if(externalSignal()) {
                Serial.println("drive to return home");
                currentState = STATE_RETURN_HOME;
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
