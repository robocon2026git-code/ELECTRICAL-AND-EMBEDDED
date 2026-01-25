/*
 * locomotion.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */

#ifndef INC_LOCOMOTION_H_
#define INC_LOCOMOTION_H_

#include "stm32f4xx_hal.h"
#include "user.h"

#define m1_dir_pin						GPIO_PIN_12
#define m2_dir_pin						GPIO_PIN_13
#define m3_dir_pin						GPIO_PIN_14
#define m4_dir_pin						GPIO_PIN_15

#define m1_pwm_pin						TIM_CHANNEL_1	//PC6
#define m2_pwm_pin						TIM_CHANNEL_2	//PB5
#define m3_pwm_pin						TIM_CHANNEL_3	//PB0
#define m4_pwm_pin						TIM_CHANNEL_4	//PB1


int lo_4_wheel_handler(TIM_HandleTypeDef *htim);
int lo_4_wheel_calculation(int vx, int vy, int omega);
void lo_4_wheel_run(TIM_HandleTypeDef *htim, uint16_t dir_pin, uint8_t mot_pin, float pwm);


float extern m1_pwm, m2_pwm, m3_pwm, m4_pwm;


#endif /* INC_LOCOMOTION_H_ */
