#include <Arduino.h>

#define COLOR_SENSOR A0
#define R_BITS 10
#define MAX_VOLTS 5
#define RED_PIN 2
#define BLUE_PIN 3
#define GREEN_PIN 4
#define RED_OUT 6
#define BLUE_OUT 7
#define YELLOW_OUT 5
#define DEFAULT_SAMPLE 10
#define BATTERY_IN 11
#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef enum {
        RED_t,
        BLUE_t,
        YELLOW_t, 
        BLACK_t,
} Colors;

void calibrate_color_sensor(uint8_t pin);
Colors read_color(uint8_t pin, uint8_t *rby_pins);
