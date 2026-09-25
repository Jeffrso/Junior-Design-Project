// WebSocket client: connects to WiFi and records incoming "pings"
#pragma once
#include <Arduino.h>

// Connect to WiFi & open WebSocket. Call once in setup().
void websocketSetup();

// Service the connection. Call every pass through loop(), or messages never arrive.
void websocketLoop();

// Returns true if a message (ping) arrived since the last call, then clears it.
// Each ping is only reported once, so one ping = one state transition.
bool websocketPingReceived();

// Text of the most recent message, in case we want to act on what it says later...
String websocketLastMessage();
