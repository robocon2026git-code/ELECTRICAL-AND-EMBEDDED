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

void staff_arm_setup() {
	Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
	Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1000);
	HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);

	Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, STAFF_ARM_P2_INITIAL_ANGLE);
	Servo_WriteAngle(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, STAFF_ARM_P3_INITIAL_ANGLE);


	HAL_Delay(50);
}

void staff_arm_control() {
    uint32_t now = HAL_GetTick();
    static uint32_t last_servo_time = 0;

    // --- STEPPER CONTROL (On/Off Logic) ---
    if(btnStatus.triangle && btnStatus.l1) {
        Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
        Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1000);
        HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE); // Ensure it's running
    } else if(btnStatus.cross && btnStatus.l1) {
        Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CCW);
        Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, 1000);
        HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
    } else {
        HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE); // Stop if no buttons pressed
    }

    // --- SERVO CONTROL (Smooth Incremental Logic) ---
    // Only update every 20ms for smooth 50Hz movement
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        if(btnStatus.up && btnStatus.r1) {
            current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        } else if(btnStatus.down && btnStatus.r1) {
            current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;
        }

        // Constraints: Keep angle between 0 and 180
        if (current_angle_p2 > 180.0f) current_angle_p2 = 180.0f;
        if (current_angle_p2 < 0.0f)   current_angle_p2 = 0.0f;

        // Apply to hardware
        Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
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
