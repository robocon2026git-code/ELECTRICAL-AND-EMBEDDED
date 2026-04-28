/*
 * arm.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#ifndef INC_STAFF_ARM_CONTROL_H_
#define INC_STAFF_ARM_CONTROL_H_

#include "bot.h"

#define STAFF_ARM_P2_INITIAL_ANGLE			90U
#define STAFF_ARM_P3_INITIAL_ANGLE			90U

#define STAFF_ARM_P2_MIN_ANGLE				90U
#define STAFF_ARM_P3_MIN_ANGLE				90U

#define STAFF_ARM_P2_MAX_ANGLE				180U
#define STAFF_ARM_P3_MAX_ANGLE				180U

#define STAFF_ARM_P2_STEP_ANGLE				2U
#define STAFF_ARM_P3_STEP_ANGLE				2U


#define PNEUMATIC_PORT					GPIOD
#define PNEUMATIC_PIN_1					GPIO_PIN_0		//PD0
#define PNEUMATIC_PIN_2					GPIO_PIN_1		//PD1

#define STAFF_ARM_P1_PULSE				TIM_CHANNEL_3
#define STAFF_ARM_P1_TIM_N				htim4

#define STAFF_ARM_P1_DIR_PIN			GPIO_PIN_12
#define STAFF_ARM_P1_DIR_PORT			GPIOE

#define STAFF_ARM_P2_PULSE				TIM_CHANNEL_1	//PE5
#define STAFF_ARM_P2_TIM_N				htim9

#define STAFF_ARM_P3_PULSE				TIM_CHANNEL_2	//PE6
#define STAFF_ARM_P3_TIM_N				htim9

void Pnuematic_OnOff(uint8_t pneumatic_pin, uint8_t SET_RESET);
void staff_arm_setup();
void staff_arm_control();


#endif /* INC_ARM_H_ */
