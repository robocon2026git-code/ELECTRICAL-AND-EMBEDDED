#include<WiFi.h>
#include<WiFiUdp.h>
#include "esp_system.h"
#include "locomotion.h"

typedef struct __attribute__((packed)) {
	float m1_pwm;
	float m2_pwm;
	float m3_pwm;
	float m4_pwm;
	uint8_t kfs_p2;
	uint8_t kfs_p3;
	float kfs_ToF;
	float bot_speed;
  uint8_t esp_rst_prv;
  uint8_t esp_rst_curr;
  uint8_t boot_count;
}BOT_Status_t;

BOT_Status_t war_status;

RTC_NOINIT_ATTR int last_reset_reason;
esp_reset_reason_t current_reason;
RTC_NOINIT_ATTR int boot_count;

WiFiUDP udp;

const char *ssid = "iQube";
const char *password = "WeEvolve@iQube";
const char *pc_ip = "10.1.75.205";
const int port = 12345;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  current_reason = esp_reset_reason();

  // First power-on initialization
  if (current_reason == ESP_RST_POWERON) {
    boot_count = 0;
    last_reset_reason = -1;   // no previous error
  }

  boot_count++;

  Serial.println("==== BOOT INFO ====");
  Serial.print("Boot count: ");
  Serial.println(boot_count);

  Serial.print("Previous reset reason: ");
  Serial.println(last_reset_reason);

  Serial.print("Current reset reason: ");
  Serial.println(current_reason);
  Serial.println("===================");

  // Store CURRENT reason for NEXT reboot
  last_reset_reason = current_reason;


  pinMode(p1,OUTPUT);
  pinMode(p2,OUTPUT);
  pinMode(p3,OUTPUT);
  pinMode(p4,OUTPUT);
  pinMode(pwm1,OUTPUT);
  pinMode(pwm2,OUTPUT);
  pinMode(pwm3,OUTPUT);
  pinMode(pwm4,OUTPUT);

  Setup_StreamWifi();

  // ps5.begin("14:3A:9A:91:49:EE");         //Black colour
  // ps5.begin("E8:47:3A:36:ED:CA");         //White colour
  ps5.begin("90:B6:85:64:59:2B");           //Camofledge colour
  
  while(ps5.isConnected() == false){
    Serial.println("PS5 Not Found!");
    delay(750);
  }

  Serial.println("PS5 Connected!");
}

void loop() {
  // put your main code here, to run repeatedly:
  war_status.esp_rst_prv  = last_reset_reason;
  war_status.esp_rst_curr = current_reason;
  war_status.boot_count   = boot_count;
  Send_BotStatusWifi();

  static bool restarted = false;

  // if (!restarted) {
  //   restarted = true;
  //   Serial.println("Restarting in 3 seconds...");
  //   delay(3000);
  //   esp_restart();
  // }

  bot_run();

  war_status.m1_pwm       = m1;
  war_status.m2_pwm       = m2;
  war_status.m3_pwm       = m3;
  war_status.m4_pwm       = m4;

  Send_BotStatusWifi();

  // Serial.println("M1 PWM:" + String(war_status.m1_pwm));
  // Serial.println("M2 PWM:" + String(war_status.m2_pwm));
  // Serial.println("M3 PWM:" + String(war_status.m3_pwm));
  // Serial.println("M4 PWM:" + String(war_status.m4_pwm));
}

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

  udp.write((uint8_t*)&war_status, sizeof(war_status));

  udp.endPacket();
}

