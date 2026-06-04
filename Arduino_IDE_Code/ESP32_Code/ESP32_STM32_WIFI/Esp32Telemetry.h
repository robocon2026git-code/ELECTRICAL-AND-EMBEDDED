/*
 * Esp32Telemetry.h
 * Central include hub for all ESP32 files.
 * Defines BOT_Status_t — the telemetry struct received from STM32.
 */

#ifndef ESP32TELEMETRY_H
#define ESP32TELEMETRY_H

#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <BluetoothSerial.h>
#include <ps5Controller.h>
#include <math.h>
#include "uart.h"
#include "StreamWifi.h"

// Telemetry struct sent from STM32 -> ESP32 -> WiFi UDP -> PC
// Must match the BOT_Status_t in bot.h on the STM32 side
typedef struct __attribute__((packed)) {
    uint8_t staff_p1;   // Staff arm pivot 1 position/state
    uint8_t staff_p2;   // Staff arm pivot 2 position/state
    uint8_t staff_p3;   // Staff arm pivot 3 position/state
    uint8_t kfs_p1;     // KFS arm pivot 1 position/state
    uint8_t kfs_p2;     // KFS arm pivot 2 position/state
    uint8_t kfs_p3;     // KFS arm pivot 3 position/state
    float   kfs_ToF;    // KFS time-of-flight sensor reading (mm)
    float   bot_speed;  // Overall bot speed estimate
} BOT_Status_t;

extern BOT_Status_t war_status;

#endif // ESP32TELEMETRY_H
