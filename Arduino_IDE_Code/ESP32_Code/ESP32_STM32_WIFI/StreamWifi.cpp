/*
 * StreamWifi.cpp
 * Connects to WiFi and streams BOT_Status_t telemetry to PC via UDP.
 * Change ssid, password, and pc_ip to match your network.
 */

#include "StreamWifi.h"

WiFiUDP udp;

static const char *ssid     = "iQube";
static const char *password = "WeEvolve@iQube";
static const char *pc_ip    = "10.1.75.177";
static const int   port     = 12345;

void Setup_StreamWifi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
    udp.begin(port);
}

void Send_BotStatusWifi() {
    // Send the entire war_status struct as raw bytes to the PC dashboard
    udp.beginPacket(pc_ip, port);
    udp.write((uint8_t*)&war_status, sizeof(war_status));
    udp.endPacket();
}
