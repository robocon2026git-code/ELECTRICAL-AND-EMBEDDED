#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "Esp32Telemetry.h" // Ensure this exists in your project

#define TXD2              19
#define RXD2              21
#define BUFFER_SIZE       10
#define STX               0xAA

// Increased deadzone to completely ignore the -16.00 stick drift!
#define ANALOG_DEADZONE   20 

typedef union {
  uint16_t halfword;
  struct {
    uint16_t up         :1; 
    uint16_t down       :1; 
    uint16_t left       :1; 
    uint16_t right      :1; 
    uint16_t triangle   :1; 
    uint16_t cross      :1; 
    uint16_t square     :1; 
    uint16_t circle     :1; 
    uint16_t l1         :1; 
    uint16_t r1         :1; 
    uint16_t options    :1; 
    uint16_t ps         :1; 
    uint16_t share      :1; 
    uint16_t touchpad   :1; 
    uint16_t reserved   :2; // 16 bits total
  } bits;
} ButtonField;

typedef struct __attribute__((packed)) {
    uint16_t btn_flag;   // 2 bytes
    float    lx;         // 4 bytes
    float    ly;         // 4 bytes
    float    rx;         // 4 bytes
    float    ry;         // 4 bytes
    float    l2;         // 4 bytes
    float    r2;         // 4 bytes
} Packet;

_Static_assert(sizeof(Packet) == 26, "Packet size mismatch! Must be 26 bytes.");

void notify();
void send_uart_data(const char *data);
void send_uart_analog_data(const float data);
void send_uart_val(const int val);
void send_packet(uint16_t btn_flag, float lx_val, float ly_val, float rx_val, float ry_val, float l2, float r2);
void receive_pkt();

extern BluetoothSerial SerialBT;
extern HardwareSerial commSerial;

extern ButtonField button;
extern Packet pkt;

extern uint16_t flag;
extern float lx_val;
extern float ly_val;
extern float rx_val;
extern float ry_val;
extern char tx_buffer[BUFFER_SIZE];
extern float tx_buffer_analog;

#endif