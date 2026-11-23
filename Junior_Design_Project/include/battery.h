#include <Arduino.h>
#define R_BITS 12
#define MAX_VOLTS 3.3
#define BATTERY_IN 20 // subject to change (idk what current pins are)
#define LOW_BATTERY_PIN 48


void battery_init();
float get_battery_volt();
void check_low_battery();