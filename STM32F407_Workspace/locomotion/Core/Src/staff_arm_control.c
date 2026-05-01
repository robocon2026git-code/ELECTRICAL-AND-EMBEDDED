#include "staff_arm_control.h"

int current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
int current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;

int32_t current_steps_1 = 0;
static uint32_t last_servo_time = 0;
static bool pwm_running = false;
static bool current_dir_cw = true;

void staff_arm_setup() {
    current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
    current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
    current_steps_1 = 0;

    Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
    HAL_TIM_PWM_Start(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE);

    Servo_WriteAngle(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    HAL_TIM_PWM_Start(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE);
}

void staff_arm_control() {
    uint32_t now = HAL_GetTick();

    // --- 1. NON-BLOCKING STEPPER CONTROL ---
    bool move_cw  = btnStatus.triangle;
    bool move_ccw = btnStatus.cross;

    if (move_cw || move_ccw) {
        bool target_dir = move_cw; // True if CW

        if (!pwm_running) {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, target_dir ? CW : CCW);
            current_dir_cw = target_dir;
            Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1500);
            HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = true;
        }
        else if (target_dir != current_dir_cw) {
            // Instant direction flip: stop, change, start (no HAL_Delay)
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, target_dir ? CW : CCW);
            current_dir_cw = target_dir;
            HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
        }
    } else {
        if (pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
        }
    }

    // --- 2. THROTTLED SERVO CONTROL (Every 20ms) ---
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        if (btnStatus.up)    current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.down)  current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.left)  current_angle_p3 += STAFF_ARM_P3_STEP_ANGLE;
        if (btnStatus.right) current_angle_p3 -= STAFF_ARM_P3_STEP_ANGLE;

        // Constraints
        if (current_angle_p2 > 170) current_angle_p2 = 170;
        if (current_angle_p2 < 10)  current_angle_p2 = 10;
        if (current_angle_p3 > 180) current_angle_p3 = 180;
        if (current_angle_p3 < 0)   current_angle_p3 = 0;

        Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
        Servo_WriteAngle(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    }

    // --- 3. PNEUMATIC CONTROL ---
    if (btnStatus.circle) {
        HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_RESET);
    } else if (btnStatus.square) {
        HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_SET);
    }
}
