/*
 * user.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#ifndef INC_USER_H_
#define INC_USER_H_


#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "main.h"

typedef struct __attribute__((packed)) {
    uint8_t btn_flag;   // 1 byte
    float   lx;         // 4 bytes
    float   ly;         // 4 bytes
    float   rx;         // 4 bytes
    float   ry;         // 4 bytes
} Packet;

_Static_assert(sizeof(Packet) == 17, "Packet size mismatch");


typedef struct {
    uint8_t up        :1;
    uint8_t down      :1;
    uint8_t left      :1;
    uint8_t right     :1;
    uint8_t triangle  :1;
    uint8_t cross     :1;
    uint8_t square    :1;
    uint8_t circle    :1;
}BitfieldButtonStatusUsr;


#define MAX(a,b) ((a) > (b) ? (a) : (b));

#define STX								0xAA

extern uint8_t rx_val;
extern Packet rx_pkt;
extern uint8_t ch, len;

extern float LX_usr;
extern float LY_usr;
extern float RX_usr;
extern float RY_usr;

extern BitfieldButtonStatusUsr btnStatus;

long map(long val, long in_min, long in_max, long out_min, long out_max);
uint32_t millis(void);

int bldc_maping(int val, int stop, int max_fw, int max_rw);

void recieve_uart(UART_HandleTypeDef *uart);

void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed);
void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val);

void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t angle);


#endif /* INC_USER_H_ */
