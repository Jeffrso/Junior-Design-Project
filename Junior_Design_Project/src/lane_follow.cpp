// Lane following: 3 color sensors -> position error e -> PD steering.
//   Sensors over lane (L M R) -> e:
//     0 1 0 -> 0                 centered
//     0 1 1 -> +1, 0 0 1 -> +2   lane is to our right: steer right
//     1 1 0 -> -1, 1 0 0 -> -2   lane is to our left: steer left
//     0 0 0 -> +3 or -3          lost: turn toward the side we last saw it (stop after LOST_STOP_MS)
//     1 1 1 -> 0, 1 0 1 -> keep the last e
//   u = Kp*e + Kd*(e - e_prev)
//   left wheel = base + u, right wheel = base - u (base drops when |e| is big, so it slows in curves)
//   IR sensor stops bot at the wall at the end of the lane.
#include "lane_follow.h"
#include "motor_driver.h"
#include "distance.h"

// Tuning (in LANE_TEST_MODE, these can change live: p35, d20, v130 + Enter)
static float Kp = 35; // steering per unit of error: raise if it runs off curves, lower if it zigzags
static float Kd = 0; // damping: once Kp works, raise (20 to 60) to stop overshoot
static int basePwm = 130; // forward speed when centered (0-255)
const int SLOW_PER_ERROR = 15; // base speed drops this much per unit of |e|
const unsigned long STEP_MS = 40; // one control step: color read (~6 ms) + IR read (~30 ms)
const unsigned long LOST_STOP_MS = 1500; // stop if no sensor sees the lane for this long

static bool running = false;
static Colors target = YELLOW_t;
static int ePrev = 0;
static int lastSide = 1; // +1 = lane last seen on the right, -1 = on the left
static bool lost = false;
static unsigned long lostSinceMs = 0;
static unsigned long lastStepMs = 0;
static int stepCount = 0;

static int patternToError(bool l, bool m, bool r) {
    if (!l && m && !r) return 0;
    if (!l && m && r) return 1;
    if (!l && !m && r) return 2;
    if (l && m && !r) return -1;
    if (l && !m && !r) return -2;
    if (l && m && r) return 0;
    if (!l && !m && !r) return 3 * lastSide; // lost
    return ePrev; // 1 0 1: shouldn't happen, keep going
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

    // 1. Wall ahead: stop before hitting it
    if (collisionUpdate()) {
        stopWith("wall ahead");
        return false;
    }

    // 2. Which sensors are over the target lane color?
    Colors c[3];
    colorReadAll(c);
    bool l = (c[0] == target);
    bool m = (c[1] == target);
    bool r = (c[2] == target);

    // 3. Position error
    int e = patternToError(l, m, r);
    if (e == 1 || e == 2) lastSide = 1;
    if (e == -1 || e == -2) lastSide = -1;

    // 4. Lost for too long: stop
    if (!l && !m && !r) {
        if (!lost) {
            lost = true;
            lostSinceMs = millis();
        }
        if (millis() - lostSinceMs > LOST_STOP_MS) {
            stopWith("lane lost");
            return false;
        }
    } else {
        lost = false;
    }

    // 5. PD steering
    float u = Kp * e + Kd * (e - ePrev);
    ePrev = e;
    int base = basePwm - SLOW_PER_ERROR * abs(e);
    int left = base + (int)u;
    int right = base - (int)u;
    drive(left, right);

    // print 5 times a second (printing each step would slow the loop down)
    if (++stepCount % 5 == 0) {
        Serial.printf("L M R = %d %d %d | e %+d | left %4d right %4d\n", l, m, r, e, left, right);
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
