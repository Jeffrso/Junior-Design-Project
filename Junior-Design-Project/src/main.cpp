#include <Adafruit_NeoPixel.h>
#include "web.h"
#include "color_sense.h"


//RBG OUTPUTS FOR DEBUGGING
#define RED_OUT 19
#define BLUE_OUT 20
#define GREEN_OUT 21 

#define BATTERY_IN 36


void setup() {
        Serial.begin(9600);
        init_color_sensor();
  //OUTPUT LEDS
        pinMode(BLUE_OUT, OUTPUT);
        pinMode(RED_OUT, OUTPUT);
        pinMode(GREEN_OUT, OUTPUT);
        pinMode(BATTERY_IN, INPUT);
  // webSocket_ini();
  
}

void print_LED(Colors color) {
        if (color == RED_t) {
                digitalWrite(RED_OUT, HIGH);
                digitalWrite(GREEN_OUT, LOW);
                digitalWrite(BLUE_OUT, LOW);
        } else if (color == BLUE_t) {
                digitalWrite(RED_OUT, LOW);
                digitalWrite(GREEN_OUT, LOW);
                digitalWrite(BLUE_OUT, HIGH);
        } else if (color == YELLOW_t) {
                digitalWrite(RED_OUT, LOW);
                digitalWrite(GREEN_OUT, HIGH);
                digitalWrite(BLUE_OUT, LOW);
        } else {
                digitalWrite(RED_OUT, LOW);
                digitalWrite(GREEN_OUT, LOW);
                digitalWrite(BLUE_OUT, LOW);
        }
}

void loop() {
    

  //color sensor 

  
        float voltage_value = 0;
        Colors color_new = NO_COLOR;
        Colors color_old = NO_COLOR;
        uint8_t rby_pins[3] = {RED_PIN, GREEN_PIN, BLUE_PIN};
        calibrate_color_sensor(COLOR_SENSOR); 

        while(1) {

                color_new = read_color(COLOR_SENSOR, rby_pins);

                if (color_new != color_old) color_change = true;
                else color_change = false;

                if (color_change) {
                        color_new = read_color(COLOR_SENSOR, rby_pins);

                        if (color_new != color_old) {
                                color_old = color_new;
                                color_change = false;
                        }
                }
                        delay(500);
        }



  // delay(500);  
}