#pragma once
#include <Arduino.h>

// GPIO pins for all segments
extern const uint8_t SEG_PINS[7];
const bool SEG_ACTIVE_LOW = false;

// Set segment pins as outputs and blank the display. Call once in setup().
void sevenseg_setup();

uint8_t sevenseg_decode(uint8_t bcd_in);
