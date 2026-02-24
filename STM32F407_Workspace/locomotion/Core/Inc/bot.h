/*
 * bot.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#ifndef INC_BOT_H_
#define INC_BOT_H_

#include <staff_arm_control.h>
#include "user.h"
#include "locomotion.h"


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

int system_status();

#endif /* INC_BOT_H_ */
