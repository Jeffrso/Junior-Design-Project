#include <Adafruit_NeoPixel.h>
#include "web.h"
#include "motor_driver.h"



void setup() {
  Serial.begin(115200);
  pinMode(REVERSE, OUTPUT);
  pinMode(FORWARD, OUTPUT);
  // webSocket_ini();
  
}

void loop() {
  // char* myMessage = "Hello esp32";
  // webSocket_send_message(myMessage);
  // String message = get_message();
  // Serial.println("Finished sending now message is: ");
  // Serial.println(message);
    drive_forward(60);
  

  // delay(500);
  digitalWrite(REVERSE, LOW);
  // digitalWrite(REVERSE, LOW);
  
  // // delay(500);
  // analogWrite(FORWARD, 55);
  // delay(500);

 

  // delay(500);
  // Serial.println("MOVE DARN IT");

}