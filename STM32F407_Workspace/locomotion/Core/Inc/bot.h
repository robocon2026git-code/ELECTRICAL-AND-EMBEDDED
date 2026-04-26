/*
 * bot.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#ifndef INC_BOT_H_
#define INC_BOT_H_

#include <string.h>
#include <stdbool.h>
#include "user.h"
#include "locomotion.h"
#include "kfs_arm_control.h"
#include <staff_arm_control.h>


typedef struct {
	uint8_t staff_p1;
	uint8_t staff_p2;
	uint8_t staff_p3;
	uint8_t kfs_p1;
	uint8_t kfs_p2;
	uint8_t kfs_p3;
	float kfs_ToF;
	float bot_speed;///
}BOT_Status_t;

extern BOT_Status_t war_status;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim12;

extern UART_HandleTypeDef huart2;

int system_status();
int odu();
void bot_setup();

#endif /* INC_BOT_H_ */
