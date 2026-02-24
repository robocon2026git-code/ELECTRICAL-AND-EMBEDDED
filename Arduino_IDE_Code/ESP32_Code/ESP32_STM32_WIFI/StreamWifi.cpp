#include "StreamWifi.h"

WiFiUDP udp;


const char *ssid = "iQube";
const char *password = "WeEvolve@iQube";
const char *pc_ip = "10.1.75.177";
const int port = 12345;



void Setup_StreamWifi() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  udp.begin(port);
}

void Send_BotStatusWifi() {
  udp.beginPacket(pc_ip, port);
  // udp.print("ESP32_Connected");
  // udp.print("BOT SPEED: " + String(war_status.bot_speed));
  // udp.print("KFS ToF Distance: " + String(war_status.kfs_ToF));
  // udp.print("STAFF PIVOT1: " + String(war_status.staff_p1));
  udp.write((uint8_t*)&war_status, sizeof(war_status));
  udp.endPacket();
}
