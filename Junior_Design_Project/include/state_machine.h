// State machine functions that main.cpp needs to see
#pragma once

// true = skip WiFi/websocket (Enter still cycles states)
const bool TEST_MODE = false;

void stateMachineSetup(); // call once in setup(): turns on Enter-key interrupt
void stateMachineUpdate(); // call every pass through loop()
