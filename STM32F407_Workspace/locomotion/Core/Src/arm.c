/*
 * arm.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include "arm.h"

uint8_t curr_angle = 0;

void servo_handler(TIM_HandleTypeDef *timer, uint8_t pos){
	if(curr_angle >= MAX_ANGLE){
	  curr_angle = MAX_ANGLE;
	  printf("Current Angle: %d\n", curr_angle);
	}else if(curr_angle <= MIN_ANGLE){
	  curr_angle = 0;
	  printf("Current Angle: %d\n", curr_angle);
	}

	if(pos == POS_UP){
		HAL_Delay(SERVO_DELAY);
		Servo_WriteAngle(timer, (curr_angle+=STEP_ANGLE));
	}else if(pos == POS_DOWN){
		HAL_Delay(SERVO_DELAY);
		Servo_WriteAngle(timer, (curr_angle-=STEP_ANGLE));
	}
}





void pnuematic_actuation()
{
	if(btnStatus.circle == 1){
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, SET);
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, RESET);
	}else if(btnStatus.square == 1){
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, RESET);
		HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, SET);
	}
}
