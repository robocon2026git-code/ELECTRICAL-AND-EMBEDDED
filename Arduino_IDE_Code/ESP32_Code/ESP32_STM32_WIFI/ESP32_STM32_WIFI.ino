/*
 * ESP32_STM32_WIFI.ino
 * Integrated with Spark Max BLDC control and Mode Switching (Staff/KFS)
 */

#include "Esp32Telemetry.h"
#include <ESP32Servo.h>

#define BAUD_RATE  115200
#define MOTOR_PIN  23  // Pin for Spark Max BLDC

// Servo object for BLDC
Servo sparkMax;

// Mode tracking
enum ControlMode { STAFF_MODE, KFS_MODE };
ControlMode currentMode = STAFF_MODE;
bool lastOptionsState = false;

void onConnect();
void onDisconnect();

void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(2, OUTPUT);

    // Initialize Motor
    sparkMax.attach(MOTOR_PIN, 1000, 2000);
    sparkMax.writeMicroseconds(1500); // Neutral/Arming pulse
    delay(3000);
    button.halfword = 0x00;

    commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial2 started at " + String(BAUD_RATE) + " baud");

    Set_BotStatusTemp();

    ps5.attachOnConnect(onConnect);
    ps5.attachOnDisconnect(onDisconnect);
      
      ps5.begin("14:3A:9A:91:49:EE");   // Black
    // ps5.begin("E8:47:3A:36:ED:CA");   // White
    //ps5.begin("90:B6:85:64:59:2B");      // Camouflage

    while (ps5.isConnected() == false) {
        Serial.println("PS5 Not Found");
        delay(350);
    }
    
    Serial.println("System Ready. Initial Mode: STAFF");
}

void loop() {
    if (!ps5.isConnected()) return;

    // 1. Mode Switching Logic (Toggle using Options button)
    bool currentOptions = ps5.Options();
    if (currentOptions && !lastOptionsState) {
        currentMode = (currentMode == STAFF_MODE) ? KFS_MODE : STAFF_MODE;
        Serial.print("Mode Switched to: ");
        Serial.println(currentMode == KFS_MODE ? "KFS" : "STAFF");
    }
    lastOptionsState = currentOptions;

    // 2. BLDC Motor Control (Directly from ESP32)
    if (currentMode == KFS_MODE) {
        if (ps5.Up()) {
            sparkMax.writeMicroseconds(1700); // Forward pulse[cite: 1]
            Serial.println("FORWARD");
        } 
        else if (ps5.Down()) {
            sparkMax.writeMicroseconds(1300); // Reverse pulse[cite: 1]
            Serial.println("BACKWARD");
        } 
        else {
            sparkMax.writeMicroseconds(1500); // Neutral/Stop[cite: 1]
            Serial.println("STOP");
        }
    } else {
        // Ensure motor is stopped in STAFF mode if direct control is not intended
        sparkMax.writeMicroseconds(1500);
    }

    // 3. Telemetry/UART Logic
    notify(); // Synchronizes button states and sends Packet to STM32[cite: 4, 9]
    receive_pkt(); // Optional: receive telemetry back from STM32[cite: 4, 9]
    
    delay(20);
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

void onConnect() { Serial.println("PS5 Connected"); }
void onDisconnect() { Serial.println("PS5 Disconnected"); }