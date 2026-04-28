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

typedef struct {
    uint16_t up        	:1;
    uint16_t down      	:1;
    uint16_t left      	:1;
    uint16_t right     	:1;//
    uint16_t triangle  	:1;
    uint16_t cross     	:1;
    uint16_t square    	:1;
    uint16_t circle    	:1;
    uint16_t l1        	:1;
    uint16_t r1        	:1;
    uint16_t reserved	:6;
}BitfieldButtonStatusUsr;

typedef struct __attribute__((packed)) {
    uint16_t btn_flag;   // 2 bytes
    float    lx;         // 4 bytes
    float    ly;         // 4 bytes
    float    rx;         // 4 bytes
    float    ry;         // 4 bytes
    float    l2;	     // 4 bytes
    float    r2;	     // 4 bytes
} Packet;

_Static_assert(sizeof(Packet) == 26, "Packet size mismatch");


typedef enum {
    CW = 1,
    CCW = 0
} Stepper_Dir_t;



typedef struct {
    float currentAngle;  // Current position
    float targetAngle;   // Where we want to go
    float stepSize;      // How many degrees to move per "tick" (smaller = slower)
    uint32_t speedDelay; // How many ms to wait between steps
    uint32_t lastTick;   // Timestamp of last movement
} SmoothServo_t;


#define MAX(a,b) ((a) > (b) ? (a) : (b));

#define STX								0xAA

extern uint8_t rx_val;
extern Packet rx_pkt;
extern uint8_t ch, len;

extern float LX_usr;
extern float LY_usr;
extern float RX_usr;
extern float RY_usr;
extern float L2_usr;
extern float R2_usr;

extern BitfieldButtonStatusUsr btnStatus;

long map(long val, long in_min, long in_max, long out_min, long out_max);
uint32_t millis(void);

int bldc_maping(int val, int stop, int max_fw, int max_rw);

void recieve_uart(UART_HandleTypeDef *uart);
void parse_uart_data();

void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed);
void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val);

void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t channel, uint8_t angle);

void Servo_SmoothHandler(SmoothServo_t *s, TIM_HandleTypeDef *htim, uint32_t channel);

void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse);

void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir);

void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz);


#endif /* INC_USER_H_ */
