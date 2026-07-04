/*
 * user.c
 *
 * UART state machine (interrupt-driven, 1 byte at a time).
 * Communication watchdog — stops all motors if ESP32 goes silent.
 * ACK/NACK — STM32 replies to every valid/invalid packet.
 * Reset reason — parses 0xBB frames from ESP32, prints via SWV.
 *
 * RULE: NEVER printf() inside uart_rx_callback().
 *   printf() uses ITM which can block for many microseconds.
 *   Inside an interrupt this corrupts timing and can crash the system.
 *   Instead, set flags in the interrupt and print in comm_watchdog_tick()
 *   which runs safely in the main loop.
 *
 * STATE MACHINE:
 *   WAIT_STX -> WAIT_LEN -> WAIT_DATA -> parse + send ACK -> WAIT_STX
 *   On bad length -> send NACK -> WAIT_STX (STM32 holds last state)
 *   On 0xBB frame -> parse reset reason -> set print flag -> WAIT_STX
 */

#include "user.h"

// Global variables
Packet  rx_pkt;
uint8_t ch  = 0;
uint8_t len = 0;

float LX_usr = 0;
float LY_usr = 0;
float RX_usr = 0;
float RY_usr = 0;
float L2_usr = 0;
float R2_usr = 0;

BitfieldButtonStatusUsr btnStatus;

// Set in interrupt when a new valid packet arrives, cleared in main loop
volatile uint8_t packet_valid = 0;

// UART state machine
typedef enum {
    UART_WAIT_STX = 0,
    UART_WAIT_LEN,
    UART_WAIT_DATA
} UART_State_t;

static UART_State_t uart_state        = UART_WAIT_STX;
static uint8_t      uart_rx_byte      = 0;
static uint8_t      uart_data_buf[sizeof(Packet)];
static uint8_t      uart_data_index   = 0;
static uint8_t      uart_expected_len = 0;
static uint8_t      uart_frame_type   = 0;  // STX or RST_FRAME_BYTE

// Watchdog state
static uint32_t last_valid_pkt_tick = 0;
static uint8_t  watchdog_triggered  = 0;  // prevents repeated stop calls

// Deferred print flags — set in ISR, read in main loop (safe to printf there)
volatile uint8_t  rst_flag        = 0;
volatile uint8_t  rst_reason_code = 0;

// UART handle reference
static UART_HandleTypeDef *g_uart = NULL;

// Drive motor timer (defined in main.c)
extern TIM_HandleTypeDef htim3;

// ─────────────────────────────────────────────────────────────────────────────
// uart_start_receive()
// Call once in main() after MX_USART2_UART_Init().
// Arms interrupt for first byte. Initializes watchdog timer.
// ─────────────────────────────────────────────────────────────────────────────
void uart_start_receive(UART_HandleTypeDef *uart) {
    g_uart = uart;
    last_valid_pkt_tick = HAL_GetTick();  // start watchdog from now
    HAL_UART_Receive_IT(uart, &uart_rx_byte, 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// uart_rx_callback()
// Called from HAL_UART_RxCpltCallback in stm32f4xx_it.c.
// Runs in INTERRUPT context — NO printf, NO delays, NO blocking.
// ─────────────────────────────────────────────────────────────────────────────
void uart_rx_callback(UART_HandleTypeDef *uart) {

    switch (uart_state) {

        case UART_WAIT_STX:
            if (uart_rx_byte == STX) {
                // Normal packet incoming
                uart_frame_type = STX;
                uart_state = UART_WAIT_LEN;
            } else if (uart_rx_byte == RST_FRAME_BYTE) {
                // Reset reason frame from ESP32
                uart_frame_type = RST_FRAME_BYTE;
                uart_state = UART_WAIT_LEN;
            }
            // Any other byte is garbage — stay in WAIT_STX and discard
            break;

        case UART_WAIT_LEN:
            uart_expected_len = uart_rx_byte;

            if (uart_frame_type == STX) {
                if (uart_expected_len == sizeof(Packet)) {
                    // Valid length — collect data bytes
                    uart_data_index = 0;
                    uart_state = UART_WAIT_DATA;
                } else {
                    // Wrong length — send NACK, STM32 holds last position
                    uint8_t nack = NACK_BYTE;
                    HAL_UART_Transmit(uart, &nack, 1, 5);
                    uart_state = UART_WAIT_STX;
                }
            } else if (uart_frame_type == RST_FRAME_BYTE) {
                if (uart_expected_len == 1) {
                    uart_data_index = 0;
                    uart_state = UART_WAIT_DATA;
                } else {
                    uart_state = UART_WAIT_STX;
                }
            }
            break;

        case UART_WAIT_DATA:
            uart_data_buf[uart_data_index++] = uart_rx_byte;

            if (uart_data_index >= uart_expected_len) {
                // All bytes received — process frame

                if (uart_frame_type == STX) {
                    // Copy to rx_pkt and parse into LX_usr, btnStatus etc.
                    memcpy(&rx_pkt, uart_data_buf, sizeof(Packet));
                    parse_uart_data();

                    // Send ACK — tells ESP32 packet was accepted
                    uint8_t ack = ACK_BYTE;
                    HAL_UART_Transmit(uart, &ack, 1, 5);

                    // Feed watchdog and set packet_valid flag for main loop
                    last_valid_pkt_tick = HAL_GetTick();
                    watchdog_triggered  = 0;
                    packet_valid        = 1;

                } else if (uart_frame_type == RST_FRAME_BYTE) {
                    // Store reset reason — main loop will print it via SWV
                    rst_reason_code = uart_data_buf[0];
                    rst_flag        = 1;
                }

                uart_state = UART_WAIT_STX;
            }
            break;
    }

    // Re-arm for next byte — must always be last
    HAL_UART_Receive_IT(uart, &uart_rx_byte, 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// comm_watchdog_tick()
// Call in the main while(1) loop every iteration.
//
// Does three things:
//  1. Prints ESP32 reset reason if one arrived (deferred from interrupt)
//  2. If no valid packet for COMM_WATCHDOG_MS ms, stops all motors ONCE
//  3. Bot automatically resumes when next valid packet arrives
// ─────────────────────────────────────────────────────────────────────────────
void comm_watchdog_tick(void) {

    // Print reset reason if ESP32 sent one (safe here — not in interrupt)
    if (rst_flag) {
        rst_flag = 0;
        printf("[ESP32 RESET] Reason code: %d", (int)rst_reason_code);
        switch (rst_reason_code) {
            case 1:  printf(" (POWER_ON)\r\n");    break;
            case 3:  printf(" (SOFTWARE)\r\n");    break;
            case 4:  printf(" (PANIC/CRASH)\r\n"); break;
            case 6:  printf(" (INT_WDT)\r\n");     break;
            case 7:  printf(" (TASK_WDT)\r\n");    break;
            case 9:  printf(" (BROWNOUT)\r\n");    break;
            default: printf(" (UNKNOWN)\r\n");     break;
        }
    }

    // Watchdog: check if ESP32 has gone silent
    uint32_t elapsed = HAL_GetTick() - last_valid_pkt_tick;
    if (elapsed > COMM_WATCHDOG_MS) {
        if (!watchdog_triggered) {
            // First time — stop everything and zero inputs
            all_motors_stop();
            LX_usr = 0; LY_usr = 0;
            RX_usr = 0; RY_usr = 0;
            L2_usr = 0; R2_usr = 0;
            *((uint16_t*)&btnStatus) = 0x0000;
            watchdog_triggered = 1;
            // Print only once — not every loop — so SWV stays readable
            printf("[WATCHDOG] No packet for %lums. Motors stopped.\r\n", (unsigned long)elapsed);
        }
    }
    // watchdog_triggered is cleared in uart_rx_callback when a valid packet arrives
    // so the bot resumes automatically when ESP32 reconnects
}

// ─────────────────────────────────────────────────────────────────────────────
// all_motors_stop()
// Immediately sets all 4 drive wheel PWM channels to 0.
// Also resets direction pins to LOW to prevent back-EMF issues.
// ─────────────────────────────────────────────────────────────────────────────
void all_motors_stop(void) {
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
    // Also clear direction/indicator pins on GPIOC
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
}

// ─────────────────────────────────────────────────────────────────────────────
// parse_uart_data()
// Copies rx_pkt into the global variables used by bot.c, locomotion.c etc.
// Called from interrupt — NO printf allowed here.
// ─────────────────────────────────────────────────────────────────────────────
void parse_uart_data(void) {
    // Copy button flags into bitfield struct
    *((uint16_t*)&btnStatus) = rx_pkt.btn_flag;
    LX_usr = rx_pkt.lx;
    LY_usr = rx_pkt.ly;
    RX_usr = rx_pkt.rx;
    RY_usr = rx_pkt.ry;
    L2_usr = rx_pkt.l2;
    R2_usr = rx_pkt.r2;
}

// ─────────────────────────────────────────────────────────────────────────────
// Utility functions
// ─────────────────────────────────────────────────────────────────────────────
uint32_t millis(void) { return HAL_GetTick(); }

long map(long val, long in_min, long in_max, long out_min, long out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Motor speed: 0.0 (stop) to 1.0 (full)
void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed) {
    if (speed < 0.0f) speed = 0.0f;
    if (speed > 1.0f) speed = 1.0f;
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    __HAL_TIM_SET_COMPARE(htim, channel, (uint32_t)((arr + 1) * speed));
}

// Motor speed: 0 (stop) to 255 (full)
void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val) {
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    __HAL_TIM_SET_COMPARE(htim, channel, (val * (arr + 1)) / 255);
}

// Servo: 0-180 degrees, pulse 1000-2000us
void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t channel, uint8_t angle) {
    if (angle > 180) angle = 180;
    __HAL_TIM_SET_COMPARE(timer, channel, 1000 + (angle * 1000) / 180);
}

// Servo: STM32F407 at 168MHz, PSC=167, ARR=19999 (1us/tick, 50Hz)
// Pulse range: 500us (0deg) to 2500us (180deg)
void Servo_WriteAngle_168Mhz(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t angle) {
    if (angle > 180) angle = 180;
    uint16_t pulse = 500 + ((uint32_t)angle * 2000) / 180;
    __HAL_TIM_SET_COMPARE(htim, channel, pulse);
}

// BLDC: pulse 1000-2000us. 1500 = neutral/stop.
void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse) {
    if (pulse < 1000 || pulse > 2000) return;
    __HAL_TIM_SET_COMPARE(timer, channel, pulse);
}

int bldc_maping(int val, int stop, int max_fw, int max_rw) {
    if (val == 0) return stop;
    if (val < 0)  return (int)map(val, -127, -1, max_rw, stop);
    return (int)map(val, 1, 127, stop, max_fw);
}

void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, (dir == CW) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Stepper: hz = desired step frequency. Timer must be at 1MHz tick rate.
void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz) {
    if (hz < 10) hz = 10;
    uint32_t arr = (1000000 / hz) - 1;
    __HAL_TIM_SET_AUTORELOAD(htim, arr);
    __HAL_TIM_SET_COMPARE(htim, channel, arr / 2);
    htim->Instance->EGR = TIM_EGR_UG;  // force register update immediately
}
