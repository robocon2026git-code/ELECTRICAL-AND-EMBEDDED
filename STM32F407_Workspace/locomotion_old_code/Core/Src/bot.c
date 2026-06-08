/*
 * bot.c
 *  Fixed: Removed printf from odu() mode switch
 *         Removed blocking recieve_uart() — now interrupt-driven via uart_rx_callback
 */

#include "bot.h"
#include "user.h"

BOT_Status_t war_status;

uint8_t is_staff_mode   = 1;
uint8_t last_options_state = 0;
uint32_t last_blue_blink   = 0;

int system_status() {
    war_status.staff_p1  = 0;
    war_status.staff_p2  = 0;
    war_status.staff_p3  = 0;
    war_status.kfs_p1    = 0;
    war_status.kfs_p2    = 0;
    war_status.kfs_p3    = 0;
    war_status.kfs_ToF   = 43;
    war_status.bot_speed = 0;
    return 0;
}

int odu() {

    uint8_t current_options_state = (rx_pkt.btn_flag & (1 << 10)) ? 1 : 0;

    if (current_options_state == 1 && last_options_state == 0) {
        is_staff_mode = !is_staff_mode;

        if (is_staff_mode) {
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
        }
    }
    last_options_state = current_options_state;

    // ==============================
    // 🔥 SET SPEED BASED ON MODE
    // ==============================
    if (is_staff_mode) {
        locomotion_max_pwm = 100;   // slow
    } else {
        locomotion_max_pwm = 200;   // fast
    }

    // ==============================
    // LOCOMOTION
    // ==============================
    lo_4_wheel_handler(&htim3);

    // ==============================
    // ARM CONTROL
    // ==============================
    if (is_staff_mode) {
        staff_arm_control();

        if (HAL_GetTick() - last_blue_blink >= 500) {
            HAL_GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
            last_blue_blink = HAL_GetTick();
        }
    } else {
        kfs_arm_handler();
    }

    return 0;
}

void bot_setup() {
    // printf kept here — only runs once at startup, no timing impact
    printf("STM32 Ready. Booting into STAFF MODE.\r\n");

    kfs_arm_setup();
    staff_arm_setup();

    HAL_GPIO_WritePin(LED_RED_PORT,  LED_RED_PIN,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);

    HAL_Delay(1000);
}
