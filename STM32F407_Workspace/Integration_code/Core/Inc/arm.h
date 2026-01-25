/*
 * arm.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#include "user.h"

#ifndef INC_ARM_H_
#define INC_ARM_H_

#define INITIAL_ANGLE					0
#define STEP_ANGLE						1
#define MIN_ANGLE						INITIAL_ANGLE
#define MAX_ANGLE						180
#define SERVO_DELAY						20
#define POS_UP							1
#define POS_DOWN						2

#define PNEUMATIC_PORT					GPIOD
#define PNEUMATIC_PIN_1					GPIO_PIN_0
#define PNEUMATIC_PIN_2					GPIO_PIN_1


void servo_handler(TIM_HandleTypeDef *timer, uint8_t pos);

void pnuematic_actuation();

#endif /* INC_ARM_H_ */
