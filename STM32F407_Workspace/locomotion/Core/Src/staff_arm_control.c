/*
 * arm.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include <staff_arm_control.h>

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

int staff_arm_p1_ctrl(){
	Servo_WriteAngle(timer, channel, angle)
}






/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/


uint8_t curr_angle = 0;

void servo_handler(TIM_HandleTypeDef *timer, uint8_t pos){
	if(curr_angle >= MAX_ANGLE){
	  curr_angle = MAX_ANGLE;
	  printf("Current Angle: %d\n", curr_angle);
	}
	if(curr_angle <= MIN_ANGLE){
	  curr_angle = MIN_ANGLE;
	  printf("Current Angle: %d\n", curr_angle);
	}


	switch (pos){
		case POS_UP:
			HAL_Delay(SERVO_DELAY);
			Servo_WriteAngle(timer, TIM_CHANNEL_2, (curr_angle+=STEP_ANGLE));
			break;


		case POS_DOWN:
			HAL_Delay(SERVO_DELAY);
			Servo_WriteAngle(timer, TIM_CHANNEL_2, (curr_angle-=STEP_ANGLE));
			break;


		default:
			Servo_WriteAngle(timer, TIM_CHANNEL_2, curr_angle);
	}

//	if(pos == POS_UP){
//		HAL_Delay(SERVO_DELAY);
//		Servo_WriteAngle(timer, (curr_angle+=STEP_ANGLE));
//	}
//	if(pos == POS_DOWN){
//		HAL_Delay(SERVO_DELAY);
//		Servo_WriteAngle(timer, (curr_angle-=STEP_ANGLE));
//	}
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
