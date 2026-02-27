/*
 * arm.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include "user.h"

#ifndef INC_ARM_H_
#define INC_ARM_H_

#define STAFF_ARM_P1_INITIAL_ANGLE			0U
#define STAFF_ARM_P2_INITIAL_ANGLE			0U
#define STAFF_ARM_P3_INITIAL_ANGLE			0U

#define STAFF_ARM_P1_MIN_ANGLE				0U
#define STAFF_ARM_P2_MIN_ANGLE				0U
#define STAFF_ARM_P3_MIN_ANGLE				0U

#define STAFF_ARM_P1_MAX_ANGLE				90U
#define STAFF_ARM_P2_MAX_ANGLE				90U
#define STAFF_ARM_P3_MAX_ANGLE				90U



#define INITIAL_ANGLE					0
#define STEP_ANGLE						1
#define MIN_ANGLE						INITIAL_ANGLE
#define MAX_ANGLE						180
#define SERVO_DELAY						20
#define POS_UP							1
#define POS_DOWN						2

#define PNEUMATIC_PORT					GPIOD
#define PNEUMATIC_PIN_1					GPIO_PIN_0		//PD0
#define PNEUMATIC_PIN_2					GPIO_PIN_1		//PD1


void servo_handler(TIM_HandleTypeDef *timer, uint8_t pos);

void Pnuematic_OnOff(uint8_t pneumatic_pin, uint8_t SET_RESET);

int staff_arm_p1_ctrl(uint8_t angle);
int staff_arm_p2_ctrl(uint8_t angle);
int staff_arm_p3_ctrl(uint8_t angle);



extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim12;
#endif /* INC_ARM_H_ */
