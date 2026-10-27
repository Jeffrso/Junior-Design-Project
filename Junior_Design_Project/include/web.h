#ifndef WEB_H
#define WEB_H

#include "esp32_secrets.h"
#include <WebSockets.h>
#include <WebSocketsClient.h>

void webSocketEvent(WStype_t type,uint8_t* payload,size_t length);
void webSocket_ini();
void webSocket_send_message(char *message);
String get_message();
void webSocket_loop();
int webSocket_get_command(); // 1-7 once per command from server, 0 = none
bool webSocket_send_state(int state); // posts "BITBANGER123:STATE:<state>", false if not logged in

int webSocket_get_move(); // 0-6 once per MOVE command from server, -1 = none
bool webSocket_send_move(int move); // posts "BITBANGER123:MOVE:<move>", false if not logged in

#endif
