// Motion state machine for Remotely Commanded Bot Motions.
// Separate from state_machine.cpp
#pragma once

// true = run this motion state machine, false = run original state machine
const bool MOTION_MODE = true;

void motionSetup(); // call once in setup() (instead of stateMachineSetup())
void motionUpdate(); // call every pass through loop() (instead of stateMachineUpdate())