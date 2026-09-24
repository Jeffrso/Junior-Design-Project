#include <Adafruit_NeoPixel.h>
#include "web.h"

void setup() {
  Serial.begin(115200);
  webSocket_ini();
  
}

void loop() {
  
  char* myMessage = "Hello esp32";
  webSocket_send_message(myMessage);

}