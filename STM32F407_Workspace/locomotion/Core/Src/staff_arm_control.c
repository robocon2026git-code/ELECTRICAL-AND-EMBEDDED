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
//    Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
//    Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1000);

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
    static uint32_t last_servo_time = 0;
    static uint32_t last_step_tick = 0;
    static bool pwm_running = false;
    static bool last_dir_cw = true;  // track last direction

    // --- 1. STEPPER CONTROL ---
    bool move_cw  = (btnStatus.triangle);
    bool move_ccw = (btnStatus.cross);

    if (move_cw || move_ccw) {
        bool dir_changed = (move_cw != last_dir_cw);

        // If direction changed, stop PWM first
        if (dir_changed && pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
            HAL_Delay(1);  // Small delay for driver to latch new direction
        }

        // Set direction
        if (move_cw) {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
            last_dir_cw = true;
        } else {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CCW);
            last_dir_cw = false;
        }

        // Start PWM if not running
        if (!pwm_running) {
            Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1500);
            HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = true;
        }

        // Track position
        if (now - last_step_tick >= 1) {
            last_step_tick = now;
            if (move_cw) {
                if (current_steps_1 < 671)  current_steps_1++;
            } else {
                if (current_steps_1 > -671) current_steps_1--;
            }
        }

    } else {
        // Button released — stop motor
        if (pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
        }
        target_steps_1 = current_steps_1;
    }

    // --- 2. SERVO CONTROL (Every 20ms) ---
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        if (btnStatus.up) current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.down) current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.left) current_angle_p3 += STAFF_ARM_P3_STEP_ANGLE;
        if (btnStatus.right) current_angle_p3 -= STAFF_ARM_P3_STEP_ANGLE;

        if (current_angle_p2 > 170) current_angle_p2 = 170;
        if (current_angle_p2 < 10)  current_angle_p2 = 10;
        if (current_angle_p3 > 180) current_angle_p3 = 180;
        if (current_angle_p3 < 0)   current_angle_p3 = 0;

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
