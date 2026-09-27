#include <Adafruit_NeoPixel.h>
#include "web.h"

void setup() {
  Serial.begin(115200);
  webSocket_ini();
  
}

void loop() {
  char* myMessage = "Hello esp32";
  webSocket_send_message(myMessage);
  String message = get_message();
  // Serial.println("Finished sending now message is: ");
  // Serial.println(message);
  delay(5000);

}