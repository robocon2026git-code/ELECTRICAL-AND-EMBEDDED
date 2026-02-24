#ifndef STREAMWIFI_H
#define STREAMWIFI_H

#include<WiFi.h>
#include<WiFiUdp.h>
#include "Esp32Telemetry.h"


void Setup_StreamWifi();
void Send_BotStatusWifi();


// extern const char *ssid = "iQube";
// extern const char *password = "WeEvolve@iQube";
// extern const char *pc_ip = "10.1.75.177";
// extern const int port = 12345;




#endif
