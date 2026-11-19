// Lane following with 3 color sensors (Left, Middle, Right):
//   Middle sees the lane            -> go straight (e = 0), no matter what the side sensors see
//   Middle off, Right sees the lane -> steer right (e = +2) until the middle sees it
//   Middle off, Left sees the lane  -> steer left  (e = -2) until the middle sees it
//   All 3 see black (lost)          -> 1. turn toward the side the lane was last seen (SEARCH_TURN_MS)
//                                      2. then go straight until any sensor sees the lane color again,
//                                         and the rules above turn it back onto the lane
//   u = Kp*e + Kd*(e - e_prev)
//   left wheel = base + u, right wheel = base - u  (+u = left wheel faster = turn RIGHT)
//   The base speed drops when |e| is big, so the bot slows down while turning.
#include "lane_follow.h"
#include "motor_driver.h"
#include "distance.h"

// Tuning (in LANE_TEST_MODE, these can change live: p35, d20, v130 + Enter)
static float Kp = 35;     // how hard it turns when a side sensor sees the lane
static float Kd = 0;      // damping: raise (20 to 60) if it overshoots after each turn
static int basePwm = 130; // forward speed when going straight (0-255)
const int SLOW_PER_ERROR = 15;              // base speed drops this much per unit of |e|
const unsigned long STEP_MS = 25;           // one control step (color read takes ~20 ms)
const unsigned long SEARCH_TURN_MS = 400;   // lost: how long to turn before driving straight
const unsigned long SEARCH_STOP_MS = 0;     // lost: give up after this long (0 = never, press Enter to stop)

static bool running = false;
static Colors target = YELLOW_t;
static int ePrev = 0;
static int lastSide = 1; // +1 = lane last seen on the right, -1 = on the left
static bool lost = false;
static unsigned long lostSinceMs = 0;
static unsigned long lastStepMs = 0;
static int stepCount = 0;

// At least one sensor sees the lane
static int patternToError(bool l, bool m, bool r) {
    if (m) return 0;        // middle on the lane: go straight
    if (r && !l) return 2;  // lane is to our right: steer right
    if (l && !r) return -2; // lane is to our left: steer left
    return 0;               // both sides but not the middle (wide spot): go straight
}

static void stopWith(const char* reason) {
    drive(0, 0);
    running = false;
    Serial.printf("STOP: %s\n", reason);
}

void laneFollowStart(Colors t) {
    target = t;
    ePrev = 0;
    lost = false;
    lastStepMs = 0;
    running = true;
    Serial.printf("following the %s lane (Kp %.1f, Kd %.1f, base %d)\n", colorName(t), Kp, Kd, basePwm);
}

void laneFollowStop() {
    if (running) {
        stopWith("stop command");
    } else {
        drive(0, 0);
    }
}

bool laneFollowUpdate() {
    if (!running) return false;
    if (millis() - lastStepMs < STEP_MS) return true;
    lastStepMs = millis();

    // 1. Wall ahead: stop before hitting it (only once the distance sensor works)
    if (USE_WALL_STOP && collisionUpdate()) {
        stopWith("wall ahead");
        return false;
    }

    // 2. Which sensors are over the target lane color?
    Colors c[3];
    colorReadAll(c);
    int leftIdx = SENSORS_MIRRORED ? 2 : 0;
    int rightIdx = SENSORS_MIRRORED ? 0 : 2;
    bool l = (c[leftIdx] == target);
    bool m = (c[1] == target);
    bool r = (c[rightIdx] == target);

    // 3. Remember which side the lane was on (used if we lose it)
    if (r && !l) lastSide = 1;
    if (l && !r) lastSide = -1;

    // 4. Position error
    int e;
    const char* dir;
    if (l || m || r) {
        lost = false;                          // a sensor sees the lane: normal rules
        e = patternToError(l, m, r);
        dir = (e > 0) ? "steer RIGHT" : (e < 0) ? "steer LEFT" : "straight";
    } else {
        if (!lost) {                           // all 3 just turned black
            lost = true;
            lostSinceMs = millis();
        }
        unsigned long lostMs = millis() - lostSinceMs;
        if (SEARCH_STOP_MS > 0 && lostMs > SEARCH_STOP_MS) {
            stopWith("lane lost");
            return false;
        }
        if (lostMs < SEARCH_TURN_MS) {
            e = 3 * lastSide;                  // a. turn toward where we last saw the lane
            dir = (lastSide > 0) ? "lost: TURN R" : "lost: TURN L";
        } else {
            e = 0;                             // b. then straight until a sensor sees the lane
            dir = "lost: STRAIGHT";
        }
    }

    // 5. Steering
    float u = Kp * e + Kd * (e - ePrev);
    ePrev = e;
    int base = basePwm - SLOW_PER_ERROR * abs(e);
    int left = base + (int)u;
    int right = base - (int)u;
    drive(left, right);

    // print 5 times a second (printing each step would slow the loop down)
    if (++stepCount % 5 == 0) {
        Serial.printf("L M R = %d %d %d | e %+d %-14s | left %4d right %4d\n", l, m, r, e, dir, left, right);
    }
    return true;
}

// LANE_TEST_MODE
static void handleCommand(String cmd) {
    cmd.trim();
    cmd.toLowerCase();
    if (cmd == "" || cmd == "s") {
        laneFollowStop();
        return;
    }
    if (cmd == "y") { laneFollowStart(YELLOW_t); return; }
    if (cmd == "r") { laneFollowStart(RED_t); return; }
    if (cmd == "b") { laneFollowStart(BLUE_t); return; }

    char k = cmd.charAt(0);
    if (cmd.length() > 1 && (k == 'p' || k == 'd' || k == 'v')) {
        float value = cmd.substring(1).toFloat();
        if (k == 'p') Kp = value;
        if (k == 'd') Kd = value;
        if (k == 'v') basePwm = (int)value;
        Serial.printf("Kp %.1f, Kd %.1f, base %d\n", Kp, Kd, basePwm);
        return;
    }
    Serial.println("y / r / b = follow that lane, Enter = stop, p35 / d20 / v130 = set Kp / Kd / base speed");
}

void laneTestLoop() {
    static String line = "";
    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n') {
            handleCommand(line);
            line = "";
        } else if (ch != '\r') {
            line += ch;
        }
    }

    static bool autoStarted = false;
    if (AUTO_START_COLOR != NO_COLOR && !autoStarted && millis() > 4000) {
        autoStarted = true;
        laneFollowStart(AUTO_START_COLOR);
    }

    laneFollowUpdate();
}
