/*
 * StreamWifi.h
 * Sends BOT_Status_t over WiFi UDP to PC dashboard.
 */

#ifndef STREAMWIFI_H
#define STREAMWIFI_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include "Esp32Telemetry.h"

void Setup_StreamWifi();    // Connect to WiFi, start UDP
void Send_BotStatusWifi();  // Send war_status struct to PC

#endif
