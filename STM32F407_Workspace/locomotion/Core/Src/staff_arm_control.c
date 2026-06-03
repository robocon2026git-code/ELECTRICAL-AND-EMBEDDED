/*
 * staff_arm_control.c
 *
 * Stepper : TIM4 CH3 (PB8)
 *           1/8 microstep → 1600 steps/rev
 *           14:1 gear ratio
 *           180° max = 11200 steps
 *           Speed: 1500 Hz
 *
 * Servo P2: TIM9 CH1 (PE5) — Up/Down with R1
 * Servo P3: TIM9 CH2 (PE6) — Left/Right with R1
 *
 * Button Mapping:
 *   UP   + L1  → Stepper CW
 *   DOWN + L1  → Stepper CCW
 *   UP   + R1  → Servo P2 increase angle
 *   DOWN + R1  → Servo P2 decrease angle
 *   LEFT + R1  → Servo P3 increase angle
 *   RIGHT+ R1  → Servo P3 decrease angle
 *   CIRCLE     → Pneumatic PD0=ON,  PD1=OFF
 *   SQUARE     → Pneumatic PD0=OFF, PD1=ON
 */

#include "staff_arm_control.h"

// --- Globals ---
int     current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
int     current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
int32_t target_steps_1   = 0;
int32_t current_steps_1  = 0;

// -----------------------------------------------------------------------
void staff_arm_setup(void) {
    current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
    current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
    target_steps_1   = 0;
    current_steps_1  = 0;

    // Init Servo P2
//    Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
//    HAL_TIM_PWM_Start(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE);
    HAL_Delay(150);

    // Init Servo P3
    Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    HAL_TIM_PWM_Start(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE);
    HAL_Delay(150);
}

// -----------------------------------------------------------------------
void staff_arm_control(void) {
    uint32_t now = HAL_GetTick();

    static uint32_t last_servo_time = 0;
    static uint32_t last_step_tick  = 0;
    static bool     pwm_running     = false;
    static bool     last_dir_cw     = true;

    // ----------------------------------------------------------------
    // 1. STEPPER — runs while button held, stops instantly on release
    // ----------------------------------------------------------------
    bool move_ccw  = (btnStatus.triangle);
    bool move_cw = (btnStatus.cross);

    if (move_cw || move_ccw) {

        // Check if direction changed
        bool dir_changed = (move_cw != last_dir_cw);

        // Stop PWM briefly on direction change so driver latches new DIR
        if (dir_changed && pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
            HAL_Delay(1);
        }

        // Set direction BEFORE starting PWM
        if (move_cw) {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
            last_dir_cw = true;
        } else {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CCW);
            last_dir_cw = false;
        }

        // Start PWM only if not already running
        if (!pwm_running) {
            Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, STAFF_ARM_STEPPER_SPEED_HZ);
            HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = true;
        }

        // Software position tracking at 1ms intervals
        if (now - last_step_tick >= 1) {
            last_step_tick = now;

            if (move_cw) {
                if (current_steps_1 < STAFF_ARM_STEP_LIMIT) {
                    current_steps_1++;
                } else {
                    // CW physical limit reached — stop
                    HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
                    pwm_running = false;
                }
            } else {
                if (current_steps_1 > -STAFF_ARM_STEP_LIMIT) {
                    current_steps_1--;
                } else {
                    // CCW physical limit reached — stop
                    HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
                    pwm_running = false;
                }
            }
        }

    } else {
        // Button released — stop immediately, hold position
        if (pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
        }
        target_steps_1 = current_steps_1;
    }

    // ----------------------------------------------------------------
    // 2. SERVO CONTROL — every 20ms
    // ----------------------------------------------------------------
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        // P2 Up/Down
       /* if (btnStatus.down) current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.up) current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;
*/
        // P3 Left/Right
        if (btnStatus.left) current_angle_p3 += STAFF_ARM_P3_STEP_ANGLE;
        if (btnStatus.right) current_angle_p3 -= STAFF_ARM_P3_STEP_ANGLE;

//        // Clamp P2
//        if (current_angle_p2 > STAFF_ARM_P2_MAX_ANGLE) current_angle_p2 = STAFF_ARM_P2_MAX_ANGLE;
//        if (current_angle_p2 < STAFF_ARM_P2_MIN_ANGLE) current_angle_p2 = STAFF_ARM_P2_MIN_ANGLE;

        // Clamp P3
        if (current_angle_p3 > STAFF_ARM_P3_MAX_ANGLE) current_angle_p3 = STAFF_ARM_P3_MAX_ANGLE;
        if (current_angle_p3 < STAFF_ARM_P3_MIN_ANGLE) current_angle_p3 = STAFF_ARM_P3_MIN_ANGLE;

        // Write
//        Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
        Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    }


    Pnuematic_OnOff();
}

// -----------------------------------------------------------------------
void Pnuematic_OnOff(void) {
    static bool last_circle = false;
    static bool pneumatic_state = false;  // false = OFF, true = ON

    bool current_circle = (btnStatus.circle == 1);

    // Toggle only on button press (rising edge)
    if (current_circle && !last_circle) {
        pneumatic_state = !pneumatic_state;

        if (pneumatic_state) {
            // ON
            HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_RESET);
        } else {
            // OFF
            HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_SET);
        }
    }

    last_circle = current_circle;
}
