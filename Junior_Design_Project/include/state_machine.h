// State machine functions that main.cpp needs to see
#pragma once

// TEST MODE: fakes a ping every TEST_PING_MS so the robot steps through every
// state on its own (no WiFi/server). Set to false to use websocket pings.
const bool TEST_MODE = false;
const unsigned long TEST_PING_MS = 1500;

// Run one step of the state machine. Call every pass through loop().
void stateMachineUpdate();
