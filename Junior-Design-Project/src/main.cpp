#include <Adafruit_NeoPixel.h>
#include "motor_driver.h"



void setup() {
  Serial.begin(115200);
  pinMode(REVERSE, OUTPUT);
  pinMode(FORWARD, OUTPUT);
  pinMode(REVERSE_2, OUTPUT);
  pinMode(FORWARD_2, OUTPUT);
  
}
void loop() {
    
  //motor testing
  drive_forward(0, 200);

  drive_forward(1, 200);
  delay(500);  
}
