#include <Arduino.h>
#define R_BITS 10
#define MAX_VOLTS 5
#define BATTERY_IN 35 // subject to change (idk what current pins are)
#define LOW_BATTERY_PIN 36


void battery_init();
float get_battery_volt();
void check_low_battery();