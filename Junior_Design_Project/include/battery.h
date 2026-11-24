#include <Arduino.h>
#define R_BITS 12
#define MAX_VOLTS 3.3
#define BATTERY_IN 8 // subject to change (idk what current pins are)
#define LOW_BATTERY_PIN 41
#define GOOD_BATTERY_PIN 42


void battery_init();
float get_battery_volt();
void check_low_battery();