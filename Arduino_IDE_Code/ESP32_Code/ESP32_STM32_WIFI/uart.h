/*
 * uart.h
 *
 * PROTOCOL FRAME (ESP32 -> STM32):
 *   [0xAA][0x1A][26 bytes Packet]
 *
 * RESET REASON FRAME (ESP32 -> STM32 on every boot):
 *   [0xBB][0x01][reason_byte]
 *
 * ACK/NACK (STM32 -> ESP32 after each packet):
 *   [0xAC] = ACK  — STM32 accepted and acted on the packet
 *   [0x15] = NACK — STM32 rejected (bad length), holding last position
 *
 * HEARTBEAT:
 *   Even when nothing changes, a packet is sent every HEARTBEAT_MS.
 *   This keeps the STM32 watchdog (300ms) fed while buttons are held.
 */

#ifndef UART_H
#define UART_H

#include <Arduino.h>

// UART2 pins
#define TXD2             19   // ESP32 TX -> STM32 RX
#define RXD2             21   // ESP32 RX <- STM32 TX

// Protocol bytes
#define STX              0xAA  // Normal packet start
#define RST_FRAME_BYTE   0xBB  // Reset reason frame start
#define ACK_BYTE         0xAC  // STM32 -> ESP32: accepted
#define NACK_BYTE        0x15  // STM32 -> ESP32: rejected (NAK in ASCII)

// Timing
#define HEARTBEAT_MS     200  // Send packet every 200ms even if nothing changed
#define ACK_TIMEOUT_MS    30  // Wait max 30ms for STM32 ACK

// Analog stick deadzone — ignores drift below this value
#define ANALOG_DEADZONE   20

#define BUFFER_SIZE       10

// Button bitfield — bit order MUST match STM32 BitfieldButtonStatusUsr exactly
typedef union {
    uint16_t halfword;
    struct {
        uint16_t up       : 1;  // bit 0
        uint16_t down     : 1;  // bit 1
        uint16_t left     : 1;  // bit 2
        uint16_t right    : 1;  // bit 3
        uint16_t triangle : 1;  // bit 4
        uint16_t cross    : 1;  // bit 5
        uint16_t square   : 1;  // bit 6
        uint16_t circle   : 1;  // bit 7
        uint16_t l1       : 1;  // bit 8
        uint16_t r1       : 1;  // bit 9
        uint16_t options  : 1;  // bit 10 — used for mode switch on STM32
        uint16_t ps       : 1;  // bit 11
        uint16_t share    : 1;  // bit 12
        uint16_t touchpad : 1;  // bit 13
        uint16_t reserved : 2;  // bits 14-15, always 0
    } bits;
} ButtonField;

// Packet struct — must match STM32 Packet exactly (26 bytes)
typedef struct __attribute__((packed)) {
    uint16_t btn_flag;  // 2 bytes
    float    lx;        // 4 bytes — left stick X  (-127 to +127)
    float    ly;        // 4 bytes — left stick Y  (-127 to +127)
    float    rx;        // 4 bytes — right stick X (-127 to +127)
    float    ry;        // 4 bytes — right stick Y (-127 to +127)
    float    l2;        // 4 bytes — L2 trigger    (0 to 255)
    float    r2;        // 4 bytes — R2 trigger    (0 to 255)
} Packet;

_Static_assert(sizeof(Packet) == 26, "Packet size mismatch! Must be 26 bytes.");

// Function declarations
void notify();          // Read PS5, send packet on change OR heartbeat
void send_packet(uint16_t btn_flag, float lx, float ly,
                 float rx, float ry, float l2, float r2);
void send_zero_packet();           // Emergency stop — all fields zero
void receive_pkt();                // Read BOT_Status_t telemetry from STM32
void send_uart_data(const char *data);
void send_uart_val(const int val);
void send_uart_analog_data(const float data);

// Extern globals
// extern BluetoothSerial SerialBT;
extern HardwareSerial  commSerial;
extern ButtonField     button;
extern Packet          pkt;
extern uint16_t        flag;
extern float           lx_val, ly_val, rx_val, ry_val, l2_val, r2_val;
extern char            tx_buffer[BUFFER_SIZE];
extern float           tx_buffer_analog;

#endif // UART_H
