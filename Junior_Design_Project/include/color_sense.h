#include <Arduino.h>

typedef enum {
        RED_t,
        BLUE_t,
        YELLOW_t, 
        BLACK_t,
} Colors;

void calibrate_color_sensor(uint8_t pin);
Colors read_color(uint8_t pin, uint8_t *rby_pins);
