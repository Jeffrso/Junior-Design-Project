// Partner demo; we're Bot 2
#pragma once

// true = run partner demo (instead of motion states or state machine)
const bool PARTNER_MODE = true;

// true = stop for obstacles (IR sensor) while driving
const bool PARTNER_OBSTACLE_STOP = true;

void partnerSetup(); // call once in setup()
void partnerUpdate(); // call every pass through loop()
