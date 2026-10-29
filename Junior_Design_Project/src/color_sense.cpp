#define COLOR_SENSOR A0
#define R_BITS 10
#define MAX_VOLTS 5
#define RED_PIN 2
#define BLUE_PIN 3
#define GREEN_PIN 4
#define RED_OUT 6
#define BLUE_OUT 7
#define YELLOW_OUT 5
#define DEFAULT_READS 10
#define min(a,b) (((a) < (b)) ? (a) : (b))


#include "color_sense.h"



const float RED_CAL[3] = {0.25, 0.25};
const float BLUE_CAL[3] = {0.5, 0.25};
const float YELLOW_CAL[3] = {0.8, 0.9};
const float BLACK_CAL[3] = {0.08, 0.1};


 

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(COLOR_SENSOR, INPUT);


}


float distance_squared(float *read_value, const float *ref_value) {
        float distance = 0;

        for (int i = 0; i < 2; i++) {
                float difference = (ref_value[i] - read_value[i]);
                distance += difference * difference;
        }

        return distance;
}

float average_color_read(uint8_t pin) {
      float sensor_val[DEFAULT_READS] = {0};
      uint16_t raw_value = 0;
      float average_read = 0;

       for (int i = 0; i < DEFAULT_READS; i++) {
          raw_value = analogRead(pin);
          sensor_val[i] = ((float)raw_value / ((1 << R_BITS) - 1)) * MAX_VOLTS;
          delay(1);
      }

       for (int i = 0; i < DEFAULT_READS; i++) {
          average_read += sensor_val[i];
      }

        average_read = average_read / DEFAULT_READS;
      return average_read;
}

float calibrate_color_sensor(uint8_t pin) {
      return average_color_read(pin);
}

Colors read_color(uint8_t pin, uint8_t *rby_pins) {
        uint16_t raw_value = 0;
        Colors color;
        //values ordered (red, blue, yellow)
        char *ORDERING[2] = {"RED ", "BLUE "}; 
        float voltage_values[2] = {0};
        for (int i = 0; i < 2; i++) {
                digitalWrite(rby_pins[i], HIGH);
                voltage_values[i] = average_color_read(pin);
                // Serial.println(volts);
                delay(100);
                digitalWrite(rby_pins[i], LOW);
                delay(100);
        }
      
        for (int i = 0; i < 2; i++) {
            Serial.print(ORDERING[i]);
            Serial.println(voltage_values[i]);
        }

        float dr = distance_squared(voltage_values, RED_CAL);
        float db = distance_squared(voltage_values, BLUE_CAL);
        float dy = distance_squared(voltage_values, YELLOW_CAL);
        float dd = distance_squared(voltage_values, BLACK_CAL);

        float min_distance = min(dy, min(dd, min(dr, db)));
        // min_distance = min(min_distance, dy);
        // min_distance = min(min_distance, dd);

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
    // uint8_t rby_pins[2] = {RED_PIN, BLUE_PIN};
    // Colors color = read_color(COLOR_SENSOR, rby_pins);
    // //  print_LED(color);
    digitalWrite(RED_PIN, HIGH);
    voltage_value = average_color_read(COLOR_SENSOR);

    Serial.print("Voltage reading: ");
    Serial.println(voltage_value);
    delay(1000);
  }



}
