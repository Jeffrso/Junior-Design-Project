
#include "battery.h"
#define DEFAULT_SAMPLE 10


float average_read(uint8_t pin, uint8_t sample_size) {
      float sensor_val = 0;
      uint16_t raw_value = 0;
      float average_read = 0;

       for (int i = 0; i < sample_size; i++) {
          raw_value = analogRead(pin);
        //   Serial.print("real raw_value: ");
        //   Serial.println(raw_value);
          sensor_val = (((float)raw_value / ((1 << R_BITS) - 1))) * (float)MAX_VOLTS;
          average_read += sensor_val;
          delay(1);
      }

      average_read = average_read / sample_size;
      if (average_read < 0) {
        return 0;
      } 

      return average_read;
}

void battery_init() {
        pinMode(BATTERY_IN, INPUT);
        pinMode(LOW_BATTERY_PIN, OUTPUT);
        pinMode(GOOD_BATTERY_PIN, OUTPUT);
}

float get_battery_volt() {
        float battery_raw = average_read(BATTERY_IN, DEFAULT_SAMPLE);
        // Serial.println(battery_raw);
        return 2.73 * battery_raw;
}

void check_low_battery() {
        float curr_voltage = get_battery_volt();
        Serial.print("curr voltage: ");
        Serial.println(curr_voltage);
        if (curr_voltage < 6.5) {
                digitalWrite(LOW_BATTERY_PIN, HIGH);
                digitalWrite(GOOD_BATTERY_PIN, LOW);
        } else {
                digitalWrite(LOW_BATTERY_PIN, LOW);
                digitalWrite(GOOD_BATTERY_PIN, HIGH);
        }
}