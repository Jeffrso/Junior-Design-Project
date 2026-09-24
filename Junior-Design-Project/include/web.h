
#ifndef WEB_H
#define WEB_H

#include "esp32_secrets.h"
#include <WebSockets.h>
#include <WebSocketsClient.h>


// Error with global variable definitoin without 'static' keyword V
// Moved to Main file for success
// // Network Configuration
// // Update to tufts_eecs and network password
// const char* WIFI_SSID = SECRET_SSID;
// #define WIFI_SSID SECRET_SSID
// const char* WIFI_PASSWORD = SECRET_PASS;
// const static char* SERVER_IP = "10.5.9.24"; 
// const char* SERVER_IP = "10.5.9.24";  // IP of server ESP32
// const uint16_t SERVER_PORT = 80;
// const char* SERVER_PATH = "/ws";

// const char* CLIENT_ID = "BITBANGER123";

// WebSocketsClient webSocket;

// bool authenticated = false;
// unsigned long lastSendTime = 0;


void webSocketEvent(WStype_t type,uint8_t* payload,size_t length);
void webSocket_ini();
void webSocket_send_message(char *message);

#endif


// Error:
/*

Linking .pio\build\esp32s3_n32r16v\firmware.elf
c:/users/amouz/.platformio/...-N32R16V/include/web.h:18: multiple definition of `CLIENT_ID'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:18: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:20: multiple definition of `webSocket'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:20: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:22: multiple definition of `authenticated'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:22: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:12: multiple definition of `WIFI_PASSWORD'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:12: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:11: multiple definition of `WIFI_SSID'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:11: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:16: multiple definition of `SERVER_PATH'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:16: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:14: multiple definition of `SERVER_IP'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:14: first defined here
c:/users/amouz/.platformio/...-N32R16V/include/web.h:23: multiple definition of `lastSendTime'; .pio\build\esp32s3_n32r16v\src\main.cpp.o:C:\Users\amouz\Junior_Design\Junior-Design-Project\ESP32-S3-DevKitC-1-N32R16V/include/web.h:23: first defined here
collect2.exe: error: ld returned 1 exit status
*** [.pio\build\esp32s3_n32r16v\firmware.elf] Error 1
================================================== [FAILED] Took 5.75 seconds ==================================================

*/