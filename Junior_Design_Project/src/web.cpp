#include "web.h"

// Network Configuration
// Update to tufts_eecs and network password
const char* WIFI_SSID = SECRET_SSID;
const char* WIFI_PASSWORD = SECRET_PASS;

const char* SERVER_IP = "10.5.9.24"; // IP of server ESP32
const uint16_t SERVER_PORT = 80;
const char* SERVER_PATH = "/ws";

const char* CLIENT_ID = "BITBANGER123";
String message= "";

WebSocketsClient webSocket;

bool authenticated = false;
unsigned long lastSendTime = 0;
bool newPing = false; // set when a server message comes in, cleared when the state machine reads it

void webSocketEvent( WStype_t type, uint8_t* payload, 
                    size_t length) 
  {

  switch (type) {
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket server");

      // First message must be an approved ID
      webSocket.sendTXT(CLIENT_ID);
      break;

    case WStype_TEXT: {
      message = "";

      for (size_t i = 0; i < length; i++) {
        message += (char)payload[i];
      }

      Serial.print("Received: ");
      Serial.println(message);

      if (message.indexOf("\"authenticated\"") >= 0 &&
          message.indexOf("\"ok\"") >= 0) {
        authenticated = true;
        Serial.println("Client authenticated");
      }

      if (message.indexOf("\"error\"") >= 0) {
        authenticated = false;
        Serial.println("Authentication failed");
      }

      // Only messages for us (containing our ID) count as a ping, so other teams' traffic
      // is ignored. Login messages never count ("authenticat" catches both
      // "authentication_required" and "authenticated").
      if (message.indexOf(CLIENT_ID) >= 0 &&
          message.indexOf("authenticat") < 0 &&
          message.indexOf("\"error\"") < 0) {
        newPing = true;
      }

      break;
    }

    case WStype_DISCONNECTED:
      authenticated = false;
      Serial.println("Disconnected");
      break;

    case WStype_ERROR:
      Serial.println("WebSocket error");
      break;

    default:
      break;
  }
}

void webSocket_ini() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected");

    webSocket.begin(SERVER_IP, SERVER_PORT, SERVER_PATH);

    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
    webSocket.enableHeartbeat(15000, 3000, 2);
}

void webSocket_send_message(char *message) {
        webSocket.loop();
        if (authenticated && millis() - lastSendTime >= 5000) 
        {
          lastSendTime = millis();
          // what do you want to send?
          webSocket.sendTXT(message);
        }
}

String get_message()
{
  return message;
}

// Call every pass through loop() so messages come in right away
void webSocket_loop() {
  webSocket.loop();
}

// True once for each new server message, so one message = one state change
bool webSocket_ping_received() {
  if (newPing) {
    newPing = false;
    return true;
  }
  return false;
}