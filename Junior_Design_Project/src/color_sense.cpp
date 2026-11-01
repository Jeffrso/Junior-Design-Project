#include "color_sense.h"



const float RED_CAL[3] = {0.35, 0.17, 0.26};
const float BLUE_CAL[3] = {0.12, 0.83, 2.11};
const float YELLOW_CAL[3] = {0.95, 1.73, 0.77};
const float BLACK_CAL[3] = {0.06, 0.12, 0.22};
float brightness_offset = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(COLOR_SENSOR, INPUT);
  pinMode(BATTERY_IN, INPUT);


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
          delay(10);
      }

      average_read = average_read / sample_size;
      return average_read;
}


void calibrate_color_sensor(uint8_t pin) {
      brightness_offset = average_color_read(pin, DEFAULT_SAMPLE);
}


Colors read_color(uint8_t pin, uint8_t *rby_pins) {
        uint16_t raw_value = 0;
        Colors color;
        //values ordered (red, blue, yellow)
        char *ORDERING[3] = {"RED ", "GREEN ","BLUE "}; //used for debugging

        float voltage_values[3] = {0};
        for (int i = 0; i < 3; i++) {
                digitalWrite(rby_pins[i], HIGH);
                voltage_values[i] = average_color_read(pin, DEFAULT_SAMPLE);
                digitalWrite(rby_pins[i], LOW);
                delay(100);
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


void print_LED(Colors color) {
   if (color == RED_t) {
     digitalWrite(RED_OUT, HIGH);
     digitalWrite(YELLOW_OUT, LOW);
     digitalWrite(BLUE_OUT, LOW);
   } else if (color == BLUE_t) {
     digitalWrite(RED_OUT, LOW);
     digitalWrite(YELLOW_OUT, LOW);
     digitalWrite(BLUE_OUT, HIGH);
   } else if (color == YELLOW_t) {
     digitalWrite(RED_OUT, LOW);
     digitalWrite(YELLOW_OUT, HIGH);
     digitalWrite(BLUE_OUT, LOW);
   } else {
     digitalWrite(RED_OUT, LOW);
     digitalWrite(YELLOW_OUT, LOW);
     digitalWrite(BLUE_OUT, LOW);
   }
}


void loop() {

  float voltage_value = 0;

  while(1) {

    uint8_t rby_pins[3] = {RED_PIN, GREEN_PIN, BLUE_PIN};
    Colors color = read_color(COLOR_SENSOR, rby_pins);

      Serial.print("DETECTED: ");
      if (color == RED_t) {
        Serial.println("RED");
      } else if (color == BLUE_t) {
         Serial.println("BLUE");
      } else if (color == YELLOW_t) {
          Serial.println("YELLOW");
      } else {
          Serial.println("BLACK");
      }

    delay(1000);

  }
}
