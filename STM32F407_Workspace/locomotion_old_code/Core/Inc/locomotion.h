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
#include "bot.h"

#define ESC_MIN     1000
#define ESC_MAX     2000
#define ESC_NEUTRAL 1500

#define m1_dir_pin						GPIO_PIN_1		//PC1
#define m2_dir_pin						GPIO_PIN_2		//PC2
#define m3_dir_pin						GPIO_PIN_8		//PC8
#define m4_dir_pin						GPIO_PIN_9		//PC9

#define m1_ind_pin						GPIO_PIN_12		//PC1
#define m2_ind_pin						GPIO_PIN_13		//PC2
#define m3_ind_pin						GPIO_PIN_14		//PC8
#define m4_ind_pin						GPIO_PIN_15		//PC9

#define m1_pwm_pin						TIM_CHANNEL_1	//PC6
#define m2_pwm_pin						TIM_CHANNEL_2	//PB5
#define m3_pwm_pin						TIM_CHANNEL_3	//PB0
#define m4_pwm_pin						TIM_CHANNEL_4	//PB1

#define TRACK_LOCOMOTION_ERR			10U

#define	TRACK_BLDC_MIN_PULSE			1000U
#define TRACK_BLDC_NEUTRAL				1500U
#define	TRACK_BLDC_MAX_PULSE			2000U

#define TRACK_BLDC_SPEED				200U

int lo_4_wheel_handler(TIM_HandleTypeDef *htim);
int lo_4_wheel_calculation(int vx, int vy, int omega);
void lo_4_wheel_run(TIM_HandleTypeDef *htim, uint16_t dir_pin, uint8_t mot_pin, float pwm, uint16_t ind_pin);

int track_run();

//BLDC C
void lo_4_wheel_run_bldc(TIM_HandleTypeDef *htim, uint8_t esc_channel, float pwm);
void esc_set_pulse_us(TIM_HandleTypeDef *htim, uint8_t channel, uint16_t pulse_us);


float extern m1_pwm, m2_pwm, m3_pwm, m4_pwm;

extern int locomotion_max_pwm;


#endif /* INC_LOCOMOTION_H_ */
