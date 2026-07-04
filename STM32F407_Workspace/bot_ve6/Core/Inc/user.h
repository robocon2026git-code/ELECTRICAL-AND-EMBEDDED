/*
 * user.h
 *
 * UART protocol, watchdog, ACK/NACK, and all motor/servo utility declarations.
 *
 * KEY ADDITIONS vs original:
 * - NACK_BYTE / ACK_BYTE definitions
 * - RST_FRAME_BYTE for reset reason detection
 * - COMM_WATCHDOG_MS: STM32 stops all motors if no valid packet for 300ms
 * - comm_watchdog_tick(): call in main while(1) loop
 * - all_motors_stop(): stops all 4 drive wheels immediately
 * - packet_valid flag: set by interrupt, read by main loop safely
 */

#ifndef INC_USER_H_
#define INC_USER_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

// Button bitfield — must match ESP32 ButtonField bit order exactly
typedef struct {
    uint16_t up        : 1;
    uint16_t down      : 1;
    uint16_t left      : 1;
    uint16_t right     : 1;
    uint16_t triangle  : 1;
    uint16_t cross     : 1;
    uint16_t square    : 1;
    uint16_t circle    : 1;
    uint16_t l1        : 1;
    uint16_t r1        : 1;
    uint16_t options   : 1;  // bit 10 — mode switch
    uint16_t ps        : 1;
    uint16_t share     : 1;
    uint16_t touchpad  : 1;
    uint16_t reserved  : 2;
} BitfieldButtonStatusUsr;

// Packet struct — must match ESP32 Packet exactly (26 bytes packed)
typedef struct __attribute__((packed)) {
    uint16_t btn_flag;
    float    lx;
    float    ly;
    float    rx;
    float    ry;
    float    l2;
    float    r2;
} Packet;

_Static_assert(sizeof(Packet) == 26, "Packet size mismatch");

typedef enum { CW = 1, CCW = 0 } Stepper_Dir_t;

typedef struct {
    float    currentAngle;
    float    targetAngle;
    float    stepSize;
    uint32_t speedDelay;
    uint32_t lastTick;
} SmoothServo_t;

#define MAX(a,b) ((a) > (b) ? (a) : (b))

// Protocol bytes — must match ESP32 uart.h exactly
#define STX              0xAA  // Normal packet start byte
#define RST_FRAME_BYTE   0xBB  // Reset reason frame from ESP32
#define ACK_BYTE         0xAC  // STM32 -> ESP32: packet accepted
#define NACK_BYTE        0x15  // STM32 -> ESP32: packet rejected

// Communication watchdog timeout (ms)
// If no valid packet received for this long, all motors stop.
// Must be > ESP32 HEARTBEAT_MS (200ms). Set to 300ms.
#define COMM_WATCHDOG_MS  300

// Extern globals
extern Packet  rx_pkt;
extern uint8_t ch, len;

extern float LX_usr;
extern float LY_usr;
extern float RX_usr;
extern float RY_usr;
extern float L2_usr;
extern float R2_usr;

extern BitfieldButtonStatusUsr btnStatus;

// volatile flag set in interrupt, read in main loop
// 1 = new valid packet arrived since last odu() call
extern volatile uint8_t packet_valid;

// UART interrupt-driven API
void uart_start_receive(UART_HandleTypeDef *uart);
void uart_rx_callback(UART_HandleTypeDef *uart);

// Watchdog — call every loop iteration in main()
void comm_watchdog_tick(void);

// Stop all 4 drive motors immediately (PWM = 0)
void all_motors_stop(void);

// Parse packet into LX_usr, LY_usr, btnStatus etc.
void parse_uart_data(void);

// Utility
long     map(long val, long in_min, long in_max, long out_min, long out_max);
uint32_t millis(void);
int      bldc_maping(int val, int stop, int max_fw, int max_rw);

// Motor control
void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed);
void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val);

// Servo control
void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t channel, uint8_t angle);
void Servo_WriteAngle_168Mhz(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle);
void Servo_SmoothHandler(SmoothServo_t *s, TIM_HandleTypeDef *htim, uint32_t channel);

// BLDC / Stepper
void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse);
void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir);
void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz);

#endif /* INC_USER_H_ */
