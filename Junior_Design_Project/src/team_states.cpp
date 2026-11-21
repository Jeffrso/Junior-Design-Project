#include <Arduino.h>
#include "team_states.h"
#include "web.h"
#include "motor_driver.h"
#include "distance.h"
#include "state_machine.h" // TEST_MODE (no WiFi)

static const uint8_t RGB_LED_PIN = 38; // built-in RGB LED
static const unsigned long MOVE_MS = 5000; // drive forward this long (time spent blocked doesn't count)
static const unsigned long BLINK_MS = 100; // green blink while moving
static const unsigned long FLASH_MS = 200; // done signal: 200 ms on, 200 ms off
static const int DONE_FLASHES = 3;
static const char* MSG_TO_BOT1 = "DJKP2"; // Bot 1 drives when it sees this

enum PartnerState { WAIT, MOVE, BLOCK, FLASH, SEND };
static const char* STATE_NAMES[] = {"WAIT", "MOVE", "BLOCK", "FLASH", "SEND"};

static PartnerState state = WAIT;
static unsigned long stateStartMs = 0;
static unsigned long moveLeftMs = 0; // driving time still left
static bool fakeObstacle = false;

// Only writes to LED when the color changes
static void setLed(uint8_t r, uint8_t g, uint8_t b) {
    static uint32_t last = 0xFFFFFFFF;
    uint32_t c = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    if (c != last) {
        last = c;
        neopixelWrite(RGB_LED_PIN, r, g, b);
    }
}

static void goTo(PartnerState s, const char* why) {
    state = s;
    stateStartMs = millis();
    Serial.printf("[%s] %s\n", STATE_NAMES[s], why);
}

// Serial Monitor keys. Returns true when Enter was pressed alone (fake Bot 1 message)
static bool readKeys() {
    static String line = "";
    bool fakeMessage = false;
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\r') continue;
        if (c != '\n') {
            line += c;
            continue;
        }
        line.trim();
        line.toLowerCase();
        if (line == "") {
            fakeMessage = true;
        } else if (line == "o") {
            fakeObstacle = !fakeObstacle;
            Serial.printf("fake obstacle %s\n", fakeObstacle ? "ON" : "OFF");
        } else if (line == "s") {
            drive(0, 0);
            goTo(WAIT, "stop key");
        } else {
            Serial.println("Enter = fake Bot 1 message | o = fake obstacle on/off | s = stop");
        }
        line = "";
    }
    return fakeMessage;
}

void partnerSetup() {
    drive(0, 0);
    goTo(WAIT, "Bot 2 ready: waiting for BITBANGER:MOVE:1 (or press Enter to fake it)");
}

void partnerUpdate() {
    bool fake = readKeys();
    bool gotMove = webSocket_got_partner_move() || fake; // checked every loop, so old messages never pile up
    bool blocked = fakeObstacle || (PARTNER_OBSTACLE_STOP && obstacleAhead());
    unsigned long t = millis() - stateStartMs; // time in this state

    if (gotMove && state != WAIT) {
        Serial.println("got BITBANGER:MOVE:1 while busy: ignored");
    }

    switch (state) {
        case WAIT:
            if (TEST_MODE || webSocket_is_authenticated()) {
                setLed(0, 0, 20); // blue: ready
            } else {
                setLed(20, 0, 0); // red: not logged in to the server yet
            }
            if (gotMove) {
                moveLeftMs = MOVE_MS;
                drive(DRIVE_PWM, DRIVE_PWM);
                goTo(MOVE, "got BITBANGER:MOVE:1 -> driving forward 5 s");
            }
            break;

        case MOVE:
            setLed(0, ((t / BLINK_MS) % 2 == 0) ? 50 : 0, 0); // blink green
            if (t >= moveLeftMs) {
                drive(0, 0);
                goTo(FLASH, "5 s done -> stop, flash LED");
            } else if (blocked) {
                drive(0, 0);
                moveLeftMs -= t; // remember how much driving is left
                goTo(BLOCK, "obstacle ahead -> stopped, waiting for it to clear");
            }
            break;

        case BLOCK:
            setLed(50, 0, 0); // solid red
            if (!blocked) {
                drive(DRIVE_PWM, DRIVE_PWM);
                goTo(MOVE, "clear -> driving the rest of the 5 s");
            }
            break;

        case FLASH: {
            uint8_t v = ((t / FLASH_MS) % 2 == 0) ? 50 : 0;
            setLed(v, v, v); // white flashes
            if (t >= 2 * FLASH_MS * DONE_FLASHES) {
                setLed(0, 0, 0);
                goTo(SEND, "flashed -> telling Bot 1 to go");
            }
            break;
        }

        case SEND:
            if (TEST_MODE) {
                goTo(WAIT, "(TEST_MODE, no WiFi) would send DJKP2 now");
            } else if (webSocket_send_text(MSG_TO_BOT1)) {
                goTo(WAIT, "sent DJKP2 -> Bot 1 should drive now");
            } else if (t >= 1000) {
                stateStartMs = millis(); // try again every second
                Serial.println("not logged in to the server, retrying...");
            }
            break;
    }
}
