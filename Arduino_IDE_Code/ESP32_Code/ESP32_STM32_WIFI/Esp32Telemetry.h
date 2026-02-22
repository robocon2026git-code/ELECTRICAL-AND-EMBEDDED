#ifndef ESP32TELEMETRY_H
#define ESP32TELEMETRY_H


#define BUTTON_DEBOUNCING_DELAY   50

#define ANALOG_ERROR      2

#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <BluetoothSerial.h>
#include <ps5Controller.h>
#include <math.h> // for fabsf()
#include "uart.h"
#include "CustomBt.h"
#include "StreamWifi.h"


typedef struct __attribute__((packed)) {
  uint8_t esp_rst_prv;
  uint8_t esp_rst_curr;
  uint8_t boot_count;
  float m1_pwm;
	float m2_pwm;
	float m3_pwm;
	float m4_pwm;
	uint8_t staff_p1;
	uint8_t staff_p2;
	uint8_t staff_p3;
	uint8_t kfs_p1;
	uint8_t kfs_p2;
	uint8_t kfs_p3;
	float kfs_ToF;
	float bot_speed;
}BOT_Status_t;

extern BOT_Status_t war_status;

#endif
