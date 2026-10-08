// TEMP TEST (TODO: delete this file & two "TEMP" lines in main.cpp after testing websockets)
// Press Enter in the Serial Monitor: robot sends "BITBANGER123:next" to server
// Server echoes it back, and that echo (which contains our ID) moves the state
// Every state change goes through the websocket!
#include <Arduino.h>
#include "web.h"

extern bool authenticated;          // from web.cpp
extern unsigned long lastSendTime;  // from web.cpp

void wsEchoTestLoop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c != '\n') {
      continue;   // one send per Enter
    }
    if (!authenticated) {
      Serial.println("[test] not authenticated yet, wait for 'Client authenticated'");
    } else if (millis() - lastSendTime < 5000) {
      Serial.println("[test] web.cpp only sends once every 5 s, press Enter again in a moment");
    } else {
      char msg[] = "BITBANGER123:next";
      webSocket_send_message(msg);
      Serial.println("[test] sent BITBANGER123:next, waiting for the server's echo...");
    }
  }
}
