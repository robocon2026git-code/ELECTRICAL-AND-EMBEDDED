/*
 * ESP32_STM32_WIFI.ino
 *
 * ISSUES FIXED:
 * 1. ESP32 reset causes STM32 to keep running last command
 *    → STM32 now has a 300ms watchdog. If no packet arrives, all motors stop.
 *
 * 2. ESP32 disconnects from PS5 — reason unknown
 *    → delay(3000) was blocking BEFORE ps5.begin(), starving BT FreeRTOS tasks.
 *    → UART opened AFTER a 500ms settle delay so BT stack initializes cleanly.
 *    → Spark Max arming done AFTER PS5 connects, not before.
 *
 * 3. PS5 connected (blue LED on) but ESP32 not reacting to commands
 *    → notify() only sent when something changed. If you hold a button steady,
 *      no packet was sent → STM32 watchdog fired → bot stopped.
 *    → Fixed by adding a 200ms heartbeat: packet sent every 200ms regardless.
 *
 * 4. Mode change: all motors must stop before switching
 *    → Handled on STM32 side in bot.c: mode switch zeroes all inputs and
 *      stops drive motors + Spark (1500) before activating new mode handler.
 *
 * 5. PS5 library untouched — no changes made to ps5Controller files.
 *
 * 6. ESP32 reset reason sent to STM32 on every boot via special 0xBB frame.
 *    STM32 prints it via SWV ITM printf.
 */

#include "Esp32Telemetry.h"
#include "esp_system.h"   // for esp_reset_reason()

#define BAUD_RATE  115200

// PS5 Controller MAC addresses — comment/uncomment the one you use
#define PS5_MAC   "14:3A:9A:91:49:EE"   // Black
// #define PS5_MAC  "E8:47:3A:36:ED:CA"  // White
// #define PS5_MAC  "90:B6:85:64:59:2B"  // Camouflage

// Forward declarations
void onConnect();
void onDisconnect();
void Set_BotStatusTemp();
void send_reset_reason();

void setup() {
    Serial.begin(BAUD_RATE);

    // LED on GPIO2 — indicates PS5 connection status
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Zero the button state so no garbage is in the first packet
    button.halfword = 0x00;

    // --- UART to STM32 ---
    // Wait 500ms BEFORE opening UART2.
    // This gives the ESP32 BT stack time to initialize its FreeRTOS tasks
    // internally before UART2 starts receiving bytes from STM32.
    // Without this delay, boot noise from STM32 corrupts BT semaphores
    // and causes the xQueueSemaphoreTake crash.
    delay(500);
    commSerial.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);

    // Flush any garbage bytes STM32 sent while ESP32 was booting
    delay(100);
    while (commSerial.available()) commSerial.read();
    Serial.println("UART2 ready (ESP32 <-> STM32)");

    // Send ESP32 reset reason to STM32 so it can log via SWV
    send_reset_reason();

    // Initialize telemetry struct to safe zero values
    Set_BotStatusTemp();

    // --- PS5 Bluetooth ---
    // Register callbacks BEFORE ps5.begin() so they are wired up
    // before the BT stack starts. This is required for the library to work.
    ps5.attachOnConnect(onConnect);
    ps5.attachOnDisconnect(onDisconnect);

    // Start BT pairing. This launches internal FreeRTOS BT tasks.
    // Do NOT call delay() or block between here and the isConnected() loop —
    // that starves the BT tasks and causes the 3-connect dropout bug.
    ps5.begin(PS5_MAC);

    Serial.println("Waiting for PS5 controller...");
    while (!ps5.isConnected()) {
        Serial.println("PS5 Not Found");
        delay(300);
    }
    // PS5 is now connected — onConnect() has already fired
    Serial.println("=== System Ready ===");
}

void loop() {
    // If PS5 disconnects mid-use, send a zero packet to stop the bot,
    // then wait. The bot will remain stopped until PS5 reconnects.
    if (!ps5.isConnected()) {
        static uint32_t last_zero = 0;
        // Send zero packet every 200ms while disconnected to keep watchdog fed
        // (so STM32 doesn't lose sync when PS5 reconnects)
        if (millis() - last_zero >= 200) {
            send_zero_packet();
            last_zero = millis();
        }
        delay(50);
        return;
    }

    // Read PS5, apply deadzone, send packet on change or heartbeat
    notify();

    // Read telemetry back from STM32 and forward over WiFi UDP to PC
    receive_pkt();

    // 20ms loop = 50Hz. This gives BT library time to process events.
    delay(20);
}

// --- Send ESP32 reset reason to STM32 via special 0xBB frame ---
// STM32 uart_rx_callback detects this frame and prints via SWV printf.
// Format: [0xBB][0x01][reason_code]
void send_reset_reason() {
    esp_reset_reason_t reason = esp_reset_reason();
    const char *name = "UNKNOWN";
    switch (reason) {
        case ESP_RST_POWERON:   name = "POWER_ON";     break;
        case ESP_RST_SW:        name = "SOFTWARE";     break;
        case ESP_RST_PANIC:     name = "PANIC/CRASH";  break;
        case ESP_RST_INT_WDT:   name = "INT_WDT";      break;
        case ESP_RST_TASK_WDT:  name = "TASK_WDT";     break;
        case ESP_RST_WDT:       name = "WDT";          break;
        case ESP_RST_BROWNOUT:  name = "BROWNOUT";     break;
        case ESP_RST_DEEPSLEEP: name = "DEEP_SLEEP";   break;
        default: break;
    }
    Serial.printf("Reset reason: %s (code=%d)\n", name, (int)reason);

    // Send to STM32: [0xBB][0x01][reason_byte]
    commSerial.write((uint8_t)0xBB);
    commSerial.write((uint8_t)0x01);
    commSerial.write((uint8_t)reason);
}

// --- Initialize telemetry struct to zero ---
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

// --- PS5 callbacks ---
void onConnect() {
    Serial.println("=== PS5 Connected ===");
    digitalWrite(2, HIGH);  // LED on
}

void onDisconnect() {
    Serial.println("=== PS5 Disconnected ===");
    digitalWrite(2, LOW);   // LED off
    send_zero_packet();      // Stop bot immediately on disconnect
}
