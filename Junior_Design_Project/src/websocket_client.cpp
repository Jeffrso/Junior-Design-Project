// WebSocket client stuff
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include "websocket_client.h"

const char* WIFI_SSID     = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const char* WS_HOST       = "192.168.1.100";   // server IP or hostname
const uint16_t WS_PORT    = 80;
const char* WS_PATH       = "/";

WebSocketsClient webSocket;

bool pingPending = false;   // set when a message comes in, cleared when a state reads it
String lastMessage = "";

// Called by library from inside webSocket.loop() whenever something happens
void websocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("websocket connected");
            break;
        case WStype_DISCONNECTED:
            Serial.println("websocket disconnected");
            break;
        case WStype_TEXT:
            lastMessage = String(payload, length);
            pingPending = true;
            Serial.print("websocket ping: ");
            Serial.println(lastMessage);
            break;
        default:
            break;
    }
}

void websocketSetup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("connecting to WiFi");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected, IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi not connected yet, will keep retrying");
    }

    webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
    webSocket.onEvent(websocketEvent);
    webSocket.setReconnectInterval(2000); // retry every 2s if connection drops
}

void websocketLoop() {
    webSocket.loop();
}

bool websocketPingReceived() {
    if (pingPending) {
        pingPending = false;
        return true;
    }
    return false;
}

String websocketLastMessage() {
    return lastMessage;
}
