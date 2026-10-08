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
bool webSocket_ping_received();

#endif
