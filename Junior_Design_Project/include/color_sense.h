#pragma once
#include <Arduino.h>

// true = color sensor test (motors don't move): prints all 3 sensors, and lets us calibrate
const bool COLOR_TEST_MODE = false;

// PCB pins. Order: Left, Middle, Right (sensor 1, 2, 3 on PCB)
const uint8_t COLOR_PINS[3] = {7, 8, 9}; // COLOR_SENSE_1, _2, _3 (ADC1 pins: still work with WiFi on)
const uint8_t LED_PINS[3][3] = { // each sensor's own RGB LED: {red, green, blue}
    {14, 15, 16}, // LED1 (RGB1)
    {17, 18, 21}, // LED2 (RGB2)
    {1, 2, 38} // LED3 (RGB3)
};

// Same order as reference table (CAL) in color_sense.cpp
typedef enum {
    RED_t,
    BLUE_t,
    YELLOW_t,
    BLACK_t,
    NO_COLOR
} Colors;

void colorSetup(); // call once in setup(): LED pins as outputs, all off
void colorReadAll(Colors colors[3]); // reads all 3 sensors at once (~6 ms): colors[0] Left, [1] Middle, [2] Right
const char* colorName(Colors c); // "red", "blue", "yellow", "black"
void colorTestLoop(); // used when COLOR_TEST_MODE is true
