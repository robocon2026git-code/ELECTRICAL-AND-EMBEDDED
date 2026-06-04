/*
 * uart.cpp
 *
 * KEY FIXES:
 * - Heartbeat added: packet sent every 200ms even if nothing changed.
 *   Without this, holding a button steady = no packet = STM32 watchdog fires.
 * - ACK/NACK: ESP32 waits up to 30ms for STM32 to confirm each packet.
 *   NACK means STM32 rejected it and is holding last position.
 * - send_zero_packet(): called on PS5 disconnect / ESP32 reset.
 * - Serial.printf prints every packet to Serial Monitor for debugging.
 *   Kept short (single line) so it doesn't block significantly.
 */

#include "uart.h"
#include "Esp32Telemetry.h"
#include <ps5Controller.h>

// Global definitions
BluetoothSerial SerialBT;       // Classic BT serial (unused but keeps linker happy)
HardwareSerial  commSerial(1);  // UART2 — connected to STM32

ButtonField button;
Packet      pkt;

uint16_t flag         = 0;
float    lx_val       = 0, ly_val = 0;
float    rx_val       = 0, ry_val = 0;
float    l2_val       = 0, r2_val = 0;

char  tx_buffer[BUFFER_SIZE];
float tx_buffer_analog = 0;

// ─────────────────────────────────────────────────────────────────────────────
// send_packet()
// Sends one framed packet to STM32, then waits for ACK/NACK.
// Frame: [0xAA][26][26 bytes of Packet struct]
// ─────────────────────────────────────────────────────────────────────────────
void send_packet(uint16_t btn_flag, float lx, float ly,
                 float rx, float ry, float l2, float r2) {
    // Fill packet
    pkt.btn_flag = btn_flag;
    pkt.lx = lx;  pkt.ly = ly;
    pkt.rx = rx;  pkt.ry = ry;
    pkt.l2 = l2;  pkt.r2 = r2;

    // Send: [STX][LEN][payload]
    commSerial.write((uint8_t)STX);
    commSerial.write((uint8_t)sizeof(Packet));
    commSerial.write((uint8_t*)&pkt, sizeof(Packet));

    // Wait for ACK or NACK from STM32
    // STM32 responds with 0xAC (OK) or 0x15 (rejected).
    // If no response in ACK_TIMEOUT_MS, STM32 watchdog will handle it.
    uint32_t t0 = millis();
    while ((millis() - t0) < ACK_TIMEOUT_MS) {
        if (commSerial.available()) {
            uint8_t r = commSerial.read();
            if (r == ACK_BYTE) {
                break;  // Good — STM32 accepted the packet
            } else if (r == NACK_BYTE) {
                Serial.println("[NACK] STM32 rejected packet");
                break;  // STM32 holds last safe position
            }
            // Other bytes are telemetry fragments — ignore here
        }
    }

    // Print to Serial Monitor (one compact line per packet)
    Serial.printf("FLAG:%04X LX:%5.0f LY:%5.0f RX:%5.0f RY:%5.0f L2:%4.0f R2:%4.0f\n",
                   btn_flag, lx, ly, rx, ry, l2, r2);
}

// ─────────────────────────────────────────────────────────────────────────────
// send_zero_packet()
// Sends all zeros to STM32. STM32 sees zero inputs and stops all motors.
// Called on PS5 disconnect and in loop() when PS5 is not connected.
// ─────────────────────────────────────────────────────────────────────────────
void send_zero_packet() {
    send_packet(0x0000, 0, 0, 0, 0, 0, 0);
}

// ─────────────────────────────────────────────────────────────────────────────
// receive_pkt()
// Reads BOT_Status_t telemetry from STM32 (sent after each valid packet).
// Forwards the struct over WiFi UDP to the PC dashboard.
// ─────────────────────────────────────────────────────────────────────────────
void receive_pkt() {
    if (commSerial.available() >= (int)sizeof(BOT_Status_t)) {
        commSerial.readBytes((char*)&war_status, sizeof(BOT_Status_t));
        Send_BotStatusWifi();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// notify()
// Reads all PS5 inputs every loop iteration.
// Sends a packet if: (a) any value changed, OR (b) 200ms heartbeat is due.
//
// The heartbeat is critical.
// Without it: hold a button steady → no change → no packet → STM32 watchdog
// fires after 300ms → bot stops → looks like ESP32 is not responding.
// ─────────────────────────────────────────────────────────────────────────────
void notify() {
    // Read all 14 PS5 buttons into the bitfield
    button.bits.up       = ps5.data.button.up;
    button.bits.down     = ps5.data.button.down;
    button.bits.left     = ps5.data.button.left;
    button.bits.right    = ps5.data.button.right;
    button.bits.triangle = ps5.data.button.triangle;
    button.bits.cross    = ps5.data.button.cross;
    button.bits.square   = ps5.data.button.square;
    button.bits.circle   = ps5.data.button.circle;
    button.bits.l1       = ps5.data.button.l1;
    button.bits.r1       = ps5.data.button.r1;
    button.bits.options  = ps5.data.button.options; // triggers mode switch on STM32
    button.bits.ps       = ps5.data.button.ps;
    button.bits.share    = ps5.data.button.share;
    button.bits.touchpad = ps5.data.button.touchpad;
    button.bits.reserved = 0; // always zero

    // Apply deadzone to analog sticks — suppress values below ANALOG_DEADZONE
    lx_val = (fabsf(ps5.data.analog.stick.lx)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.lx  : 0.0f;
    ly_val = (fabsf(ps5.data.analog.stick.ly)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.ly  : 0.0f;
    rx_val = (fabsf(ps5.data.analog.stick.rx)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.rx  : 0.0f;
    ry_val = (fabsf(ps5.data.analog.stick.ry)  > ANALOG_DEADZONE) ? ps5.data.analog.stick.ry  : 0.0f;
    l2_val = (fabsf(ps5.data.analog.button.l2) > ANALOG_DEADZONE) ? ps5.data.analog.button.l2 : 0.0f;
    r2_val = (fabsf(ps5.data.analog.button.r2) > ANALOG_DEADZONE) ? ps5.data.analog.button.r2 : 0.0f;

    // 0xFFFF initial value forces the very first packet to always send
    static uint16_t last_flag    = 0xFFFF;
    static float    last_lx = 999, last_ly = 999, last_rx = 999;
    static float    last_ry = 999, last_l2 = 999, last_r2 = 999;
    static uint32_t last_send_ms = 0;

    uint32_t now = millis();

    bool changed = (button.halfword != last_flag ||
                    lx_val != last_lx || ly_val != last_ly ||
                    rx_val != last_rx || ry_val != last_ry ||
                    l2_val != last_l2 || r2_val != last_r2);

    // Heartbeat: send every HEARTBEAT_MS even if nothing changed.
    // HEARTBEAT_MS (200) < watchdog timeout (300ms) so watchdog never fires.
    bool heartbeat = ((now - last_send_ms) >= HEARTBEAT_MS);

    if (changed || heartbeat) {
        send_packet(button.halfword, lx_val, ly_val, rx_val, ry_val, l2_val, r2_val);
        last_flag = button.halfword;
        last_lx = lx_val; last_ly = ly_val;
        last_rx = rx_val; last_ry = ry_val;
        last_l2 = l2_val; last_r2 = r2_val;
        last_send_ms = now;
    }
}

// Utility
void send_uart_data(const char *data)     { commSerial.write(data); }
void send_uart_val(const int val)         { if (val > 0) commSerial.write(val); }
void send_uart_analog_data(const float d) { commSerial.print(d); }
