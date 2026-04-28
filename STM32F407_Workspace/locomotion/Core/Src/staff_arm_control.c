/*
 * arm.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include <staff_arm_control.h>

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
int current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
int current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;

// --- Global Variables for Tracking ---
int32_t target_steps_1 = 0;
int32_t current_steps_1 = 0;
// Time tracking for the logic blocks
uint32_t last_stepper_tick = 0;
// Note: last_servo_time and last_step_increment_tick are
// usually declared as 'static' inside the function, which is also fine.

void staff_arm_setup() {
    // 1. Set Stepper Direction and Speed
    Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
    Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1000);

    // Note: Do NOT Start Stepper PWM here if you want it to wait for a button press.

    // 2. Synchronize Logic Variables with Hardware
    current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
    current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
    target_steps_1 = 0;
    current_steps_1 = 0;

    // 3. Set and Start Servo P2
    Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
    HAL_TIM_PWM_Start(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE);
    HAL_Delay(150); // Wait for P2 to reach position and current to stabilize

    // 4. Set and Start Servo P3
    Servo_WriteAngle(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    HAL_TIM_PWM_Start(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE);
    HAL_Delay(150);

    // 5. If using TIM1 or TIM8 (Advanced Timers), enable Main Output
    // __HAL_TIM_MOE_ENABLE(&STAFF_ARM_P2_TIM_N);
}

void staff_arm_control() {
    uint32_t now = HAL_GetTick();
    static uint32_t last_stepper_tick = 0;
    static uint32_t last_servo_time = 0;
    static uint32_t last_step_increment_tick = 0;

    // --- 1. TARGET UPDATE (Every 40ms) ---
    if (now - last_stepper_tick >= 40) {
        last_stepper_tick = now;

        // Stepper Target Logic
        if (btnStatus.up && btnStatus.l1)    target_steps_1 += 20;
        if (btnStatus.down && btnStatus.l1)  target_steps_1 -= 20;

        // Stepper Limits (671 steps based on your gear ratio)
        int32_t limit = 671;
        if (target_steps_1 > limit)  target_steps_1 = limit;
        if (target_steps_1 < -limit) target_steps_1 = -limit;

        // Servo P3 Target Logic (Left/Right)
        if (btnStatus.left && btnStatus.r1)  current_angle_p3 += STAFF_ARM_P3_STEP_ANGLE;
        if (btnStatus.right && btnStatus.r1) current_angle_p3 -= STAFF_ARM_P3_STEP_ANGLE;
    }

    // --- 2. STEPPER EXECUTION (Hardware Pulse Logic) ---
    if (current_steps_1 != target_steps_1) {
        uint32_t freq = 1500; // 1500 Hz
        Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, (current_steps_1 < target_steps_1) ? CW : CCW);
        Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, freq);
        HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);

        // Sync current_steps to real time (1500Hz = 1 step every ~0.66ms)
        uint32_t step_interval = 1000 / freq;
        if (now - last_step_increment_tick >= step_interval) {
            last_step_increment_tick = now;
            if (current_steps_1 < target_steps_1) current_steps_1++;
            else current_steps_1--;
        }
    } else {
        HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
    }

    // --- 3. SERVO CONTROL (Every 20ms) ---
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        // P2 (Up/Down)
        if(btnStatus.up && btnStatus.r1)   current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        if(btnStatus.down && btnStatus.r1) current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;

        // Constraints and Hardware Write
        if (current_angle_p2 > 170.0f) current_angle_p2 = 170.0f;
        if (current_angle_p2 < 10.0f)  current_angle_p2 = 10.0f;
        if (current_angle_p3 > 180.0f) current_angle_p3 = 180.0f;
        if (current_angle_p3 < 0.0f)   current_angle_p3 = 0.0f;

        Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
        Servo_WriteAngle(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    }
}

void Pnuematic_OnOff(uint8_t pneumatic_pin, uint8_t SET_RESET)
{
	if(btnStatus.circle == 1){
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, SET);
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, RESET);
	}else if(btnStatus.square == 1){
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, RESET);
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, SET);
	}
}
