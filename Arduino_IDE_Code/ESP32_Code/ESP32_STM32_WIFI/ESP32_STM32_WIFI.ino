/*
 * ESP32_STM32_WIFI.ino
 *  Fixed: Removed button.halfword = 0x00 from loop()
 *         This was destroying notify()'s state tracking and causing
 *         continuous packet flooding to STM32 every loop iteration.
 */

#include "Esp32Telemetry.h"

#define BAUD_RATE  115200

void onConnect();
void onDisconnect();

void setup() {
    Serial.begin(BAUD_RATE);

    pinMode(2, OUTPUT);

    button.halfword = 0x00;

    // Setup_StreamWifi();  // uncomment if using WiFi telemetry
    commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial2 started at " + String(BAUD_RATE) + " baud");

    Set_BotStatusTemp();
    // setup_bt();  // uncomment if using Bluetooth

    ps5.attachOnConnect(onConnect);
    ps5.attachOnDisconnect(onDisconnect);

    ps5.begin("14:3A:9A:91:49:EE");   // Black
    // ps5.begin("E8:47:3A:36:ED:CA");   // White
    // ps5.begin("90:B6:85:64:59:2B");      // Camouflage

    while (ps5.isConnected() == false) {
        Serial.println("PS5 Not Found");
        delay(350);
    }
}

void loop() {
    notify();

    // REMOVED: button.halfword = 0x00
    // This line was wiping button state every loop, making notify() think
    // buttons changed every single iteration → flooding STM32 with packets
    // → causing motor jerk. notify() handles its own state tracking internally.

    receive_pkt(); // optional: receive telemetry back from STM32
}

void Set_BotStatusTemp() {
    war_status.bot_speed = 0;
    war_status.kfs_p1    = 0;
    war_status.kfs_p2    = 0;
    war_status.kfs_p3    = 0;
    war_status.kfs_ToF   = 0;
    war_status.staff_p1  = 0;
    war_status.staff_p2  = 0;
    war_status.staff_p3  = 0;
}

void onConnect() {
    Serial.println("PS5 Connected");
}

void onDisconnect() {
    Serial.println("PS5 Disconnected");
}
