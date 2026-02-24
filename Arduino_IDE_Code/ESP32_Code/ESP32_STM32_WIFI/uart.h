#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "Esp32Telemetry.h"

#define TXD2              19
#define RXD2              21

#define BUFFER_SIZE       10

#define STX               0xAA

typedef union{
  uint8_t byte;
  struct{
    uint8_t up        :1;
    uint8_t down      :1;
    uint8_t left      :1;
    uint8_t right     :1;
    uint8_t triangle  :1;
    uint8_t cross     :1;
    uint8_t square    :1;
    uint8_t circle    :1;
  } bits;
} ButtonField;

typedef struct __attribute__((packed)) {
    uint8_t  btn_flag;     // 1 byte
    float    lx;       // 4 bytes
    float    ly;       // 4 bytes
    float    rx;       // 4 bytes
    float    ry;       // 4 bytes
} Packet;


void notify();
void send_uart_data(const char *data);
void send_uart_analog_data(const float data);
void send_uart_val(const int *val);
void send_packet(uint8_t btn_flag, float lx_val, float ly_val, float rx_val, float ry_val);
void receive_pkt();

extern BluetoothSerial SerialBT;
extern HardwareSerial commSerial;

extern ButtonField button;
extern Packet pkt;

extern uint8_t flag;
extern float lx_val;
extern float ly_val;
extern float rx_val;
extern float ry_val;

extern char tx_buffer[BUFFER_SIZE];
extern float tx_buffer_analog;



#endif
