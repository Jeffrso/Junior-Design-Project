#include "color_sense.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define DEFAULT_SAMPLE 10

const float RED_CAL[3] = {0.4020, 0.1560, 0.3600};
const float BLUE_CAL[3] = {0.3057, 0.8271, 1.9550};
const float YELLOW_CAL[3] = {1.2367, 1.5333, 1.1333};
const float BLACK_CAL[3] = {0.0767, 0.1183, 0.2600};
float brightness_offset = 0;
bool color_change = false;


void init_color_sensor() {
        //LEDs for sensor
        pinMode(BLUE_PIN, OUTPUT);
        pinMode(RED_PIN, OUTPUT);
        pinMode(GREEN_PIN, OUTPUT);
        pinMode(COLOR_SENSOR, INPUT);
}


float distance_squared(float *read_value, const float *ref_value) {
        float distance = 0;

        for (int i = 0; i < 3; i++) {
                float difference = (ref_value[i] - read_value[i]);
                distance += difference * difference;
        }

        return distance;
}


float average_color_read(uint8_t pin, uint8_t sample_size) {
      float sensor_val = 0;
      uint16_t raw_value = 0;
      float average_read = 0;

       for (int i = 0; i < sample_size; i++) {
          raw_value = analogRead(pin);
          sensor_val = ((float)raw_value / ((1 << R_BITS) - 1)) * MAX_VOLTS;
          average_read += sensor_val;
          delay(1);
      }

      average_read = average_read / sample_size;
      if (average_read < 0) {
        return 0;
      } 

      return average_read;
}


void calibrate_color_sensor(uint8_t pin) {
      brightness_offset = average_color_read(pin, DEFAULT_SAMPLE);
}


Colors read_color(uint8_t pin, uint8_t *rby_pins) {

        float voltage_values[3] = {0};
        for (int i = 0; i < 3; i++) {
                digitalWrite(rby_pins[i], HIGH);
                voltage_values[i] = average_color_read(pin, DEFAULT_SAMPLE) - brightness_offset;
                digitalWrite(rby_pins[i], LOW);
                delay(2);
        }
      

        float dr = distance_squared(voltage_values, RED_CAL);
        float db = distance_squared(voltage_values, BLUE_CAL);
        float dy = distance_squared(voltage_values, YELLOW_CAL);
        float dd = distance_squared(voltage_values, BLACK_CAL);

        float min_distance = min(dy, min(dd, min(dr, db)));

        if (dr == min_distance) return RED_t;
        else if (db == min_distance) return BLUE_t;
        else if (dy == min_distance) return YELLOW_t;
        else return BLACK_t;
}

