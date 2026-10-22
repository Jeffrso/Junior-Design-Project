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
int commandedState = 0; // 1-7 when server sends us a command, 0 = none
int commandedMove = -1; // 0-6 when server sends us a MOVE command, -1 = none

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

      // Command = "BITBANGER123:STATE:n" (n = 1-7) anywhere in server's message.
      // Other teams' web traffic don't have our ID, so it's ignored.
      String cmd = String(CLIENT_ID) + ":STATE:";
      int at = message.indexOf(cmd);
      if (at >= 0) {
        char n = message.charAt(at + cmd.length());
        if (n >= '1' && n <= '7') {
          commandedState = n - '0';
        }
      }

      // Motion command = "BITBANGER123:MOVE:m" (m = 0-6), for motion_states.cpp
      String moveCmd = String(CLIENT_ID) + ":MOVE:";
      int moveAt = message.indexOf(moveCmd);
      if (moveAt >= 0) {
        char m = message.charAt(moveAt + moveCmd.length());
        if (m >= '0' && m <= '6') {
          commandedMove = m - '0';
        }
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

// 1-7 once for each command from server, 0 if there's no new one
int webSocket_get_command() {
  int n = commandedState;
  commandedState = 0;
  return n;
}

// Post a "go to state n" command to server, e.g. "BITBANGER123:STATE:5"
bool webSocket_send_state(int state) {
  if (!authenticated) {
    return false;
  }
  String cmd = String(CLIENT_ID) + ":STATE:" + state;
  webSocket.sendTXT(cmd);
  return true;
}

// 0-6 once for each MOVE command from server, -1 if there's no new one
int webSocket_get_move() {
  int m = commandedMove;
  commandedMove = -1;
  return m;
}

// Post a "do motion m" command to server, e.g. "BITBANGER123:MOVE:1"
bool webSocket_send_move(int move) {
  if (!authenticated) {
    return false;
  }
  String cmd = String(CLIENT_ID) + ":MOVE:" + move;
  webSocket.sendTXT(cmd);
  return true;
}
