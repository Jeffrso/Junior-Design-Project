#include "team_states.h"
#include "motor_driver.h"
#include "lane_follow.h"


static const uint8_t RGB_LED_PIN = 38; // built-in RGB LED
static const unsigned long FLASH_MS = 200; // each flash is on then off for 200ms
static const unsigned long PAUSE_MS = 1000; // dark gap before flashes repeat

// State number
typedef enum {
    STATE_STOP,        // 0
    STATE_LANE_FOLLOW,     // 1
    STATE_SEND_COMMAND,     
} TeamState;

static TeamState currentState = STATE_STOP;
static TeamState nextState = STATE_STOP;
Colors target_color = BLUE_t;
String TEAM_COLAB_ID = "Djgkp";

static unsigned long stateStartMs = 0; // when we entered current state

static void goToState(int index, const char* reason) {
    currentState = (TeamState)index;
    stateStartMs = millis(); // restart flash pattern
    runCommand();
    // Serial.printf("%s -> state %d (%s)\n", reason, index, MOTION_NAMES[index]);
}


static void runCommand() {
    String message = "";
    switch (currentState) {
        case STATE_STOP:
            laneFollowStop();
            break;
        case STATE_LANE_FOLLOW:
            laneFollowStart(target_color);
            laneFollowUpdate();
            delay(5000);
            break;
        case STATE_SEND_COMMAND: 
            message = TEAM_COLAB_ID + "1";
            webSocket_send_message(message);
            break;
        default:
            laneFollowStop();;
            break;
    }
}

static void updateState() {

    switch(currentState) {
        case STATE_STOP:
            nextState = STATE_STOP;
        case STATE_LANE_FOLLOW:
            nextState = STATE_SEND_COMMAND;
        case STATE_SEND_COMMAND:
            nextState = STATE_STOP;
    }

    currentState = nextState;
}

void teamUpdate() {
    int state = webSocket_get_move();
    if (state >= 0) {
        goToState(state, "websocket");
    } else {
        updateState();
    }
}