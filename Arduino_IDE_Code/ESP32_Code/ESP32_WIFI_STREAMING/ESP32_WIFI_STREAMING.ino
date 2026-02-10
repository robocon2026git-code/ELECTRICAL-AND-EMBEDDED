#include<WiFi.h>
#include<WiFiUdp.h>

const char *ssid = "iQube";
const char *password = "WeEvolve@iQube";
const char *pc_ip = "10.1.75.177";
const int port = 12345;

void receive_msg();

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  udp.begin(port);
}

void loop() {
  // Simulate telemetry data (e.g., Battery voltage, Speed)
  String data = "BAT:12.4, SPEED:45, DIR:FWD"; 
  
  udp.beginPacket(pc_ip, port);
  udp.print("ESP32_Connected");
  //udp.print(data);
  udp.endPacket();
  receive_msg();
  delay(5000); // 10Hz update rate
}

void receive_msg() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buffer[50];
    int len =udp.read(buffer, 50);
    buffer[len] = 0;
    Serial.println(buffer);
  } 
}