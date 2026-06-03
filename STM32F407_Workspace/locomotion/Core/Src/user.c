/*
 * user.c
 *  Fixed: Removed printf from parse_uart_data (was blocking 5-20ms every packet)
 *         Changed UART to interrupt-driven (non-blocking)
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

// --- UART Interrupt State Machine ---
typedef enum {
    UART_WAIT_STX = 0,
    UART_WAIT_LEN,
    UART_WAIT_DATA
} UART_State_t;

static UART_State_t uart_state = UART_WAIT_STX;
static uint8_t      uart_rx_byte = 0;
static uint8_t      uart_data_buf[sizeof(Packet)];
static uint8_t      uart_data_index = 0;
static uint8_t      uart_expected_len = 0;

// Call once in main after MX_USART2_UART_Init()
void uart_start_receive(UART_HandleTypeDef *uart) {
    HAL_UART_Receive_IT(uart, &uart_rx_byte, 1);
}

// Call this from HAL_UART_RxCpltCallback in stm32f4xx_it.c
void uart_rx_callback(UART_HandleTypeDef *uart) {
    switch (uart_state) {
        case UART_WAIT_STX:
            if (uart_rx_byte == STX) {
                uart_state = UART_WAIT_LEN;
            }
            break;

        case UART_WAIT_LEN:
            uart_expected_len = uart_rx_byte;
            if (uart_expected_len == sizeof(Packet)) {
                uart_data_index = 0;
                uart_state = UART_WAIT_DATA;
            } else {
                uart_state = UART_WAIT_STX; // bad length, reset
            }
            break;

        case UART_WAIT_DATA:
            uart_data_buf[uart_data_index++] = uart_rx_byte;
            if (uart_data_index >= uart_expected_len) {
                memcpy(&rx_pkt, uart_data_buf, sizeof(Packet));
                parse_uart_data();
                uart_state = UART_WAIT_STX;
            }
            break;
    }

    // Re-arm for next byte
    HAL_UART_Receive_IT(uart, &uart_rx_byte, 1);
}

// --- Utility Functions ---
uint32_t millis(void) {
    return HAL_GetTick();
}

long map(long val, long in_min, long in_max, long out_min, long out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// --- UART Parse (NO printf - was causing 5-20ms block every packet) ---
void parse_uart_data() {
    *((uint16_t*)&btnStatus) = rx_pkt.btn_flag;

    LX_usr = rx_pkt.lx;
    LY_usr = rx_pkt.ly;
    RX_usr = rx_pkt.rx;
    RY_usr = rx_pkt.ry;
    L2_usr = rx_pkt.l2;
    R2_usr = rx_pkt.r2;

//    printf("BTN FLAG --> %X  |  LX -> %.2f  |  LY -> %.2f  |  RX -> %.2f  |  RY -> %.2f  |  L2 -> %.2f  |  R2 -> %2.f\n",
//    					rx_pkt.btn_flag, LX_usr, LY_usr, RX_usr, RY_usr, L2_usr, R2_usr);
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
    if (angle > 180) angle = 180;
    uint16_t pulse = 1000 + (angle * 1000) / 180;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}



// ==========================================================
// Servo_WriteAngle()
// For STM32F407 @ 168 MHz
// Timer configured as:
//   PSC = 167  → 1 MHz timer clock (1 µs per tick)
//   ARR = 19999 → 20 ms period (50 Hz servo PWM)
//
// Pulse range used: 500 µs – 2500 µs
// ==========================================================
void Servo_WriteAngle_168Mhz(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle)
{
    if (angle > 180) angle = 180;

    // 🔥 Adjust these if needed for your servo
    uint16_t min_us = 500;   // 0°
    uint16_t max_us = 2500;  // 180°

    // Convert angle → pulse width in microseconds
    uint16_t pulse = min_us + ((uint32_t)angle * (max_us - min_us)) / 180;

    // Since timer = 1 MHz → 1 tick = 1 µs
    __HAL_TIM_SET_COMPARE(htim, channel, pulse);
}





// --- BLDC Control ---
void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse) {
    if ((pulse < 1000) || (pulse > 2000)) return;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}

int bldc_maping(int val, int stop, int max_fw, int max_rw) {
    if (val == 0) return stop;
    if (val < 0)  return map(val, -127, -1, max_rw, stop);
    if (val > 0)  return map(val, 1, 127, stop, max_fw);
    return 0;
}

// --- Stepper Control ---
void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir) {
    if (dir == CW) HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    else           HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz) {
    if (hz < 10) hz = 10;
    uint32_t new_arr = (1000000 / hz) - 1;
    __HAL_TIM_SET_AUTORELOAD(htim, new_arr);
    __HAL_TIM_SET_COMPARE(htim, channel, new_arr / 2);
    htim->Instance->EGR = TIM_EGR_UG; // Force register update
}
