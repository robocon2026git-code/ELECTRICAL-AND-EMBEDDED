/*
 * bot.c
 *
 *  Created on: Jul 3, 2026
 *      Author: PRAJAN VS
 */


/*
 * bot.c
 *
 * MODE RULES (strictly enforced):
 *   STAFF MODE: Blue LED ON solid. Staff arm active. KFS actuators frozen.
 *   KFS MODE:   Blue LED OFF. KFS arm active. Staff actuators frozen.
 *
 * MODE SWITCH SEQUENCE (OPTIONS button, rising edge):
 *   1. all_motors_stop()           — stop drive wheels immediately
 *   2. Stop actuators of old mode  — Spark to 1500, stepper stopped, pneumatic off
 *   3. HAL_Delay(100)              — let Spark register neutral, servos settle
 *   4. Toggle is_staff_mode flag
 *   5. Update LEDs
 *
 * WATCHDOG:
 *   comm_watchdog_tick() called every loop.
 *   If ESP32 silent > 300ms: motors stop, inputs zeroed.
 *   When ESP32 resumes: bot automatically continues from new commands.
 *
 * IMPORTANT:
 *   odu() must NOT use HAL_Delay() except inside the mode switch sequence.
 *   HAL_Delay() blocks the main loop and breaks the watchdog timing.
 */

#include "bot.h"
#include "user.h"

BOT_Status_t war_status;

// Mode state: 1 = STAFF, 0 = KFS
uint8_t is_staff_mode      = 1;
static uint8_t last_options_state = 0;  // for rising-edge detection

// ─────────────────────────────────────────────────────────────────────────────
// stop_kfs_actuators()
// Called when leaving KFS mode. Stops Spark and offset motor.
// Servos hold their last angle passively (PWM timer keeps running).
// ─────────────────────────────────────────────────────────────────────────────
static void stop_kfs_actuators(void) {
    Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_STOP);  // Spark neutral
    motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, 0);          // Offset motor off
}

// ─────────────────────────────────────────────────────────────────────────────
// stop_staff_actuators()
// Called when leaving STAFF mode. Stops stepper and pneumatic.
// Servos P2/P3 hold their last angle passively.
// ─────────────────────────────────────────────────────────────────────────────
static void stop_staff_actuators(void) {
    HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);         // Stepper off
//    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_RESET); // Pneumatic off
//    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_RESET);
}

// ─────────────────────────────────────────────────────────────────────────────
// update_mode_leds()
// STAFF: Blue ON, Red OFF
// KFS:   Blue OFF, Red OFF
// ─────────────────────────────────────────────────────────────────────────────
static void update_mode_leds(void) {
    if (is_staff_mode) {
        HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_RED_PORT,  LED_RED_PIN,  GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_RED_PORT,  LED_RED_PIN,  GPIO_PIN_SET);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// system_status() — resets telemetry struct fields
// ─────────────────────────────────────────────────────────────────────────────
int system_status() {
    war_status.staff_p1  = 0;
    war_status.staff_p2  = 0;
    war_status.staff_p3  = 0;
    war_status.kfs_p1    = 0;
    war_status.kfs_p2    = 0;
    war_status.kfs_p3    = 0;
    war_status.kfs_ToF   = 0;
    war_status.bot_speed = 0;
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// odu() — main control loop, called inside while(1) in main.c
//
// ORDER OF OPERATIONS:
//   1. Watchdog check
//   2. Mode switch (OPTIONS button rising edge)
//   3. Locomotion (always active in both modes)
//   4. Arm control (only the active mode's arm runs)
// ─────────────────────────────────────────────────────────────────────────────
int odu() {

    // 1. WATCHDOG
    // Stops all motors if ESP32 silent for COMM_WATCHDOG_MS.
    // Prints once via SWV. Resumes automatically when ESP32 reconnects.
    comm_watchdog_tick();

    // 2. MODE SWITCH — detect rising edge of OPTIONS button (bit 10)
    uint8_t cur_opts = (rx_pkt.btn_flag & (1 << 10)) ? 1 : 0;

    if (cur_opts == 1 && last_options_state == 0) {
        printf("[MODE SWITCH] Stopping all actuators...\r\n");

        // Step 1: Stop drive wheels
        all_motors_stop();

        // Step 2: Stop actuators of the mode we are leaving
        if (is_staff_mode) {
            stop_staff_actuators();
        } else {
            stop_kfs_actuators();
        }

        // Step 3: Wait for Spark to register neutral and servos to settle
        HAL_Delay(100);

        // Step 4: Toggle mode
        is_staff_mode = !is_staff_mode;

        // Step 5: Update LEDs
        update_mode_leds();

        printf("[MODE SWITCH] Now in %s mode.\r\n", is_staff_mode ? "STAFF" : "KFS");
    }
    last_options_state = cur_opts;

    // 3. LOCOMOTION — always active in both modes
    // Speed limit differs: STAFF=100 (slower, arm deployed), KFS=200 (full speed)
    locomotion_max_pwm = is_staff_mode ? 100 : 160;
    locomotion_rotation_pwm_regular = is_staff_mode ? locomotion_rotation_pwm_regular_staff : locomotion_rotation_pwm_regular_kfs;
    locomotion_rotation_while_drive = is_staff_mode ? locomotion_rotation_while_drive_staff : locomotion_rotation_while_drive_kfs;

    lo_4_wheel_handler(&htim3);

    // 4. ARM CONTROL — only the active mode's arm handler runs
    if (is_staff_mode) {
        // KFS actuators remain frozen at their last position (passive hold)
        staff_arm_control();
    } else {
        // Staff actuators remain frozen (stepper stopped, pneumatic off)
        kfs_arm_handler();
    }

    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// bot_setup() — runs once in main() before while(1)
// ─────────────────────────────────────────────────────────────────────────────
void bot_setup() {
    printf("STM32 Ready. Booting STAFF MODE.\r\n");

    // Initialize arm subsystems (servos go to initial angles)
    kfs_arm_setup();
    staff_arm_setup();

    // Spark MAX must receive neutral on boot for ESC arming sequence
    Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_STOP);

    // Offset motor off on boot
    motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, 0);

    // Set LEDs for initial STAFF mode
    update_mode_leds();

    // 1 second wait for Spark ESC arming to complete
    HAL_Delay(1000);
    printf("Bot setup complete.\r\n");
}
