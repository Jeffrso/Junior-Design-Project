// Lane following: 3 color sensors + PD steering
#pragma once
#include <Arduino.h>
#include "color_sense.h"

// true = lane following test (no WiFi). Type in the Serial Monitor:
//   y / r / b + Enter = follow the yellow / red / blue lane
//   Enter (or s + Enter) = stop
//   p35 / d20 / v130 + Enter = set Kp / Kd / base speed without re-uploading
const bool LANE_TEST_MODE = true;

// Start by itself ~4 s after power-on or RST (for runs without USB cable).
// NO_COLOR = off (wait for a typed command). Example: const Colors AUTO_START_COLOR = YELLOW_t;
const Colors AUTO_START_COLOR = NO_COLOR;

void laneFollowStart(Colors target); // start following target lane color
void laneFollowStop();
bool laneFollowUpdate(); // call every loop(): returns false once stopped (wall, lane lost, or not started)
void laneTestLoop(); // used when LANE_TEST_MODE is true
