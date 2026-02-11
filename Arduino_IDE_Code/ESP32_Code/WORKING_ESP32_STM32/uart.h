#ifndef UART_H
#define UART_H

#include "Esp32Telemetry.h"

#define TXD2              19
#define RXD2              21

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

typedef struct {
	uint8_t staff_p1;
	uint8_t staff_p2;
	uint8_t staff_p3;
	uint8_t kfs_p1;
	uint8_t kfs_p2;
	uint8_t kfs_p3;
	float kfs_ToF;
	float bot_speed;
}BOT_Status_t;


BluetoothSerial SerialBT;
HardwareSerial commSerial(1);



#endif
