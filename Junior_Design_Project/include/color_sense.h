#include <Arduino.h>

#define COLOR_SENSOR 18
#define RED_PIN 15
#define BLUE_PIN 16
#define GREEN_PIN 17
#define R_BITS 12
#define MAX_VOLTS 5


typedef enum {
        RED_t,
        BLUE_t,
        YELLOW_t, 
        BLACK_t,
        NO_COLOR
} Colors;

bool color_change = false;

void init_color_sensor();
//calibrate color sensor to ambient light
void calibrate_color_sensor(uint8_t pin);
//Reads values from from the sensor and returns the color
Colors read_color(uint8_t pin, uint8_t *rby_pins);
void colorSenseSetup();
void colorSenseTestLoop();
