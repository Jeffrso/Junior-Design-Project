// State Machine
#include <Arduino.h>
#include "state_machine.h"
#include "web.h"

const uint8_t RGB_LED_PIN = 38; // built-in RGB LED
const unsigned long FLASH_MS = 200; // each flash is on then off for this long
const unsigned long PAUSE_MS = 1000; // dark gap before flashes repeat

// In the order the robot goes through:
typedef enum {
    STATE_INITIALIZE,       // 1
    STATE_IDLE,             // 2
    STATE_DRIVE_WALL_TURN,  // 3
    STATE_FIND_LANE,        // 4
    STATE_LANE_FOLLOW,      // 5
    STATE_TURN,             // 6
    STATE_RETURN_HOME,      // 7
    NUM_STATES              // = 7 (count, not state)
} RobotState;

const char* STATE_NAMES[NUM_STATES] = {
    "initialize", "idle", "drive wall turn", "find lane",
    "lane follow", "turn", "return home"
};

// LED color for each state
const uint8_t STATE_COLORS[NUM_STATES][3] = {
    {50, 0, 0}, // 1 red
    {0, 50, 0}, // 2 green
    {0, 0, 50}, // 3 blue
    {50, 50, 0}, // 4 yellow
    {0, 50, 50}, // 5 cyan
    {50, 0, 50}, // 6 purple
    {50, 50, 50} // 7 white
};

RobotState currentState = STATE_INITIALIZE;
unsigned long stateStartMs = 0; // when we entered current state

// Set by interrupt handler, cleared by stateMachineUpdate()
volatile bool enterPressed = false;
volatile char typedChar = 0; // key typed before Enter (0 = just Enter)

// Interrupt handler: USB serial receive interrupt calls this when keys arrive,
// so loop() never has to poll Serial. (Only sets flags)
void onSerialReceive(void* arg, esp_event_base_t base, int32_t id, void* data) {
    static char lastChar = 0;
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            typedChar = lastChar;
            lastChar = 0;
            enterPressed = true;
        } else if (c != '\r') {
            lastChar = c;
        }
    }
}

// State N: flash N times, pause, and repeat
void showState() {
    static int lastShown = -1; // only write to LED when it changes
    int n = currentState + 1;
    unsigned long flashTime = n * 2 * FLASH_MS; // n flashes, each on then off
    unsigned long t = (millis() - stateStartMs) % (flashTime + PAUSE_MS);
    bool on = (t < flashTime) && ((t / FLASH_MS) % 2 == 0);

    int shown = on ? n : 0; // 0 = LED off
    if (shown != lastShown) {
        lastShown = shown;
        if (on) {
            const uint8_t* c = STATE_COLORS[currentState];
            neopixelWrite(RGB_LED_PIN, c[0], c[1], c[2]);
        } else {
            neopixelWrite(RGB_LED_PIN, 0, 0, 0);
        }
    }
}

void goToState(int index, const char* reason) {
    currentState = (RobotState)index;
    stateStartMs = millis(); // restart flash pattern
    Serial.printf("%s -> state %d (%s)\n", reason, index + 1, STATE_NAMES[index]);
}

void stateMachineSetup() {
    Serial.onEvent(ARDUINO_HW_CDC_RX_EVENT, onSerialReceive);
}

void stateMachineUpdate() {
    // 1. Enter key (external signal)
    if (enterPressed) {
        char c = typedChar;
        enterPressed = false;

        if (c == 0) {
            goToState((currentState + 1) % NUM_STATES, "Enter");   // 7 wraps to 1
        } else if (c >= '1' && c <= '7') {
            if (webSocket_send_state(c - '0')) {
                Serial.printf("posted state %c command, waiting for the server...\n", c);
            } else {
                Serial.println("not connected to the server yet");
            }
        } else {
            Serial.println("press Enter for the next state, or type 1-7 then Enter");
        }
    }

    // 2. Websocket command: jump straight to that state
    int n = webSocket_get_command();
    if (n != 0) {
        goToState(n - 1, "websocket");
    }

    showState(); // flash LED to show which state we're in
}
