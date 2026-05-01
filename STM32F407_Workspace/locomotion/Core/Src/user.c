/*
 * user.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include "user.h"

// --- Global Variables ---
Packet rx_pkt;
uint8_t ch, len;

float LX_usr;
float LY_usr;
float RX_usr;
float RY_usr;
float L2_usr;
float R2_usr;

BitfieldButtonStatusUsr btnStatus;

// --- Utility Functions ---
uint32_t millis(void){
    return HAL_GetTick();
}

long map(long val, long in_min, long in_max, long out_min, long out_max) {
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// --- UART Communication ---
// NON-BLOCKING UART (Will not freeze motors)
void recieve_uart(UART_HandleTypeDef *uart) {
    if (HAL_UART_Receive(uart, &ch, 1, 5) != HAL_OK) return;

    if (ch == STX) {
        if (HAL_UART_Receive(uart, &len, 1, 5) == HAL_OK) {
            if (len == sizeof(Packet)) {
                if (HAL_UART_Receive(uart, (uint8_t*)&rx_pkt, len, 10) == HAL_OK) {
                    parse_uart_data();
                }
            }
        }
    }
}

void parse_uart_data() {
    // MAGIC ONE-LINER (Struct Version)
    *((uint16_t*)&btnStatus) = rx_pkt.btn_flag;

    // Analog Values
    LX_usr = rx_pkt.lx;
    LY_usr = rx_pkt.ly;
    RX_usr = rx_pkt.rx;
    RY_usr = rx_pkt.ry;
    L2_usr = rx_pkt.l2;
    R2_usr = rx_pkt.r2;

    // ==========================================
    // STM32 TERMINAL PRINTING LOGIC
    // ==========================================

    // 1. Print the Joysticks and Triggers
    printf("FLAG: %04X | L(%5.0f,%5.0f) R(%5.0f,%5.0f) | T(%5.0f,%5.0f) | BTNS: ",
           rx_pkt.btn_flag, LX_usr, LY_usr, RX_usr, RY_usr, L2_usr, R2_usr);

    // 2. Print out the names of whatever digital buttons are currently pressed
    if(btnStatus.up)       printf("UP ");
    if(btnStatus.down)     printf("DOWN ");
    if(btnStatus.left)     printf("LEFT ");
    if(btnStatus.right)    printf("RIGHT ");
    if(btnStatus.triangle) printf("TRIANGLE ");
    if(btnStatus.cross)    printf("CROSS ");
    if(btnStatus.square)   printf("SQUARE ");
    if(btnStatus.circle)   printf("CIRCLE ");
    if(btnStatus.l1)       printf("L1 ");
    if(btnStatus.r1)       printf("R1 ");
    if(btnStatus.options)  printf("OPTIONS ");
    if(btnStatus.ps)       printf("PS ");
    if(btnStatus.share)    printf("SHARE ");
    if(btnStatus.touchpad) printf("TOUCHPAD ");

    // 3. New line carriage return for the terminal
    printf("\r\n");
}

// --- DC Motor Control ---
void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed) {
    if (speed < 0.0f) speed = 0.0f;
    if (speed > 1.0f) speed = 1.0f;
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t ccr = (uint32_t)((arr + 1) * speed);
    __HAL_TIM_SET_COMPARE(htim, channel, ccr);
}

void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val) {
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t ccr = (val * (arr + 1)) / 255;
    __HAL_TIM_SET_COMPARE(htim, channel, ccr);
}

// --- Servo Control ---
void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t channel, uint8_t angle) {
    if(angle > 180) angle = 180;
    uint16_t pulse = 1000 + (angle * 1000) / 180;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}

// --- BLDC Control ---
void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse) {
    if((pulse < 1000) || (pulse > 2000)) return;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}

int bldc_maping(int val, int stop, int max_fw, int max_rw) {
    if(val == 0) return stop;
    if(val < 0)  return map(val, -127, -1, max_rw, stop);
    if(val > 0)  return map(val, 1, 127, stop, max_fw);
    return 0;
}

// --- Stepper Control ---
void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir) {
    if(dir == CW) HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    else          HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz) {
    if (hz < 10) hz = 10;
    uint32_t new_arr = (1000000 / hz) - 1;
    __HAL_TIM_SET_AUTORELOAD(htim, new_arr);
    __HAL_TIM_SET_COMPARE(htim, channel, new_arr / 2);
}
