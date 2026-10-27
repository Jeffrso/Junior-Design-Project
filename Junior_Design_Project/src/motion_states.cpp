// Motion State Machine: 7 motion states (0-6). Each state sends its motor commands when we enter it.
// Built-in RGB LED shows state: state 0 (stop) = solid red,
// state N = N flashes in that state's color, pause, repeat.
//
// Type in the Serial Monitor:
//   Enter            -> next state (6 wraps back to 0)
//   0-6, then Enter  -> posts "BITBANGER123:MOVE:<n>" to websocket server.
//                       (We only change state when that command comes back from the server)
//   s, then Enter    -> stop now (no server necessary)
#include <Arduino.h>
#include "motion_states.h"
#include "web.h"
#include "motor_driver.h"

static const uint8_t RGB_LED_PIN = 38; // built-in RGB LED
static const unsigned long FLASH_MS = 200; // each flash is on then off for 200ms
static const unsigned long PAUSE_MS = 1000; // dark gap before flashes repeat

// State number
typedef enum {
    STATE_STOP,        // 0
    STATE_FORWARD,     // 1
    STATE_BACKWARD,    // 2
    STATE_PIVOT_CW,    // 3
    STATE_PIVOT_CCW,   // 4
    STATE_TURN_RIGHT,  // 5
    STATE_TURN_LEFT,   // 6
    NUM_MOTIONS        // = 7 (count, not state)
} MotionState;

static const char* MOTION_NAMES[NUM_MOTIONS] = {
    "stop", "forward", "backward", "pivot CW",
    "pivot CCW", "right turn", "left turn"
};

// LED color for each state
static const uint8_t MOTION_COLORS[NUM_MOTIONS][3] = {
    {50, 0, 0}, // 0 red (solid)
    {0, 50, 0}, // 1 green
    {0, 0, 50}, // 2 blue
    {50, 50, 0}, // 3 yellow
    {0, 50, 50}, // 4 cyan
    {50, 0, 50}, // 5 purple
    {50, 50, 50} // 6 white
};

static MotionState currentState = STATE_STOP;
static unsigned long stateStartMs = 0; // when we entered current state

// Set by interrupt handler, cleared by motionUpdate()
static volatile bool enterPressed = false;
static volatile char typedChar = 0; // key typed before Enter (0 = just Enter)

// Interrupt handler: USB serial receive interrupt calls this when keys arrive,
// so loop() never has to poll Serial. (Only sets flags)
static void onSerialReceive(void* arg, esp_event_base_t base, int32_t id, void* data) {
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

// Motor commands for each state (left wheel, right wheel)
static void runMotors() {
    switch (currentState) {
        case STATE_STOP:
            drive(0, 0);
            break;
        case STATE_FORWARD:
            drive(DRIVE_PWM, DRIVE_PWM);
            break;
        case STATE_BACKWARD:
            drive(-DRIVE_PWM, -DRIVE_PWM);
            break;
        case STATE_PIVOT_CW:
            drive(PIVOT_PWM, -PIVOT_PWM);
            break; // left forward, right back
        case STATE_PIVOT_CCW:
            drive(-PIVOT_PWM, PIVOT_PWM);
            break; // left back, right forward
        case STATE_TURN_RIGHT:
            drive(TURN_OUTER_PWM, TURN_INNER_PWM);
            break; // right wheel is inner one
        case STATE_TURN_LEFT:
            drive(TURN_INNER_PWM, TURN_OUTER_PWM);
            break; // left wheel is the inner one
        default:
            drive(0, 0);
            break;
    }
}

// State 0: solid. State N: flash N times, pause, and repeat
static void showState() {
    static int lastShown = -1; // only write to LED when it changes
    int n = currentState;
    bool on = true; // state 0 (stop) stays on
    if (n > 0) {
        unsigned long flashTime = n * 2 * FLASH_MS; // n flashes, each on then off
        unsigned long t = (millis() - stateStartMs) % (flashTime + PAUSE_MS);
        on = (t < flashTime) && ((t / FLASH_MS) % 2 == 0);
    }

    int shown = on ? n + 1 : 0; // 0 = LED off
    if (shown != lastShown) {
        lastShown = shown;
        if (on) {
            const uint8_t* c = MOTION_COLORS[currentState];
            neopixelWrite(RGB_LED_PIN, c[0], c[1], c[2]);
        } else {
            neopixelWrite(RGB_LED_PIN, 0, 0, 0);
        }
    }
}

static void goToState(int index, const char* reason) {
    currentState = (MotionState)index;
    stateStartMs = millis(); // restart flash pattern
    runMotors();
    Serial.printf("%s -> state %d (%s)\n", reason, index, MOTION_NAMES[index]);
}

void motionSetup() {
    Serial.onEvent(ARDUINO_HW_CDC_RX_EVENT, onSerialReceive);
        Serial.printf("turns: outer PWM %d, inner PWM %d (target radius %.0f cm)\n",
                  TURN_OUTER_PWM, TURN_INNER_PWM, TURN_RADIUS_CM);
}

void motionUpdate() {
    // 1. Enter key (external signal)
    if (enterPressed) {
        char c = typedChar;
        enterPressed = false;

        if (c == 0) {
            goToState((currentState + 1) % NUM_MOTIONS, "Enter"); // 6 wraps to 0
        } else if (c >= '0' && c <= '6') {
            if (webSocket_send_move(c - '0')) {
                Serial.printf("posted move %c command, waiting for the server...\n", c);
            } else {
                Serial.println("not connected to the server yet");
            }
        } else {
            goToState(STATE_STOP, "stop key"); // s (or any other key) + Enter = stop now
        }
    }

    // 2. Websocket command: jump straight to that state
    int n = webSocket_get_move();
    if (n >= 0) {
        goToState(n, "websocket");
    }

    showState(); // flash LED to show which state we're in
}
