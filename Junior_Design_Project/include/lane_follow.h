// Lane following: 3 color sensors + steering. How it works is at the top of lane_follow.cpp.
#pragma once
#include <Arduino.h>
#include "color_sense.h"

// true = lane following test (no WiFi). Type in the Serial Monitor:
//   y / r / b + Enter = follow the yellow / red / blue lane
//   Enter (or s + Enter) = stop
//   p35 / d20 / v130 + Enter = set Kp / Kd / base speed without re-uploading
const bool LANE_TEST_MODE = false;

// Start by itself ~4 s after power-on or RST (for runs without USB cable).
// NO_COLOR = off (wait for a typed command). Example: const Colors AUTO_START_COLOR = YELLOW_t;
const Colors AUTO_START_COLOR = NO_COLOR;

// true = the bot's physical RIGHT sensor shows up as "L" in the printout (sensors mounted mirrored).
const bool SENSORS_MIRRORED = false;

// true = stop at walls with the IR distance sensor. Leave false until the distance sensor works:
// each wall check takes ~30 ms, which slows down the steering.
const bool USE_WALL_STOP = false;

void laneFollowStart(Colors target); // start following target lane color
void laneFollowStop();
bool laneFollowUpdate(); // call every loop(): returns false once stopped (wall, lane lost, or not started)
void laneTestLoop(); // used when LANE_TEST_MODE is true
