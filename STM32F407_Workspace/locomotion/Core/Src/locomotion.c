/*
 * locomotion.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */
#include "locomotion.h"


float  m1_pwm, m2_pwm, m3_pwm, m4_pwm;

unsigned long current = 0, previous = 0;



//int lo_4_wheel_handler(TIM_HandleTypeDef *timer){
//	int x = LY_usr;
//	int y = LX_usr;
//	int w = RX_usr;
//
//    if(abs(x) < 20) x = 0;
//    if(abs(y) < 20) y = 0;
//    if(abs(w) < 20) w = 0;
//
//    int vx = (x * 255) / 127;
//    int vy = (y * 255) / 127;
//    int omega = (w * 255) / 127;
//
//    lo_4_wheel_calculation(vx, vy, omega);
//
//    lo_4_wheel_run(timer, m1_dir_pin, m1_pwm_pin, m1_pwm);
//    lo_4_wheel_run(timer, m2_dir_pin, m2_pwm_pin, m2_pwm);
//    lo_4_wheel_run(timer, m3_dir_pin, m3_pwm_pin, m3_pwm);
//    lo_4_wheel_run(timer, m4_dir_pin, m4_pwm_pin, m4_pwm);
//    return 0;
//}



int lo_4_wheel_handler(TIM_HandleTypeDef *timer){
	int x = LY_usr;
	int y = LX_usr;
	int w = RX_usr;

    if(abs(x) < 20) x = 1500;
    if(abs(y) < 20) y = 1500;
    if(abs(w) < 20) w = 1500;

//    x=map(x,-127,127,1000,2000);
//    y=map(y,-127,127,1000,2000);
//    w=map(w,-127,127,1000,2000);

    int vx = (x * 1000) / 127;
    int vy = (y * 1000) / 127;
    int omega = (w * 1000) / 127;

    lo_4_wheel_calculation(vx, vy, omega);

    lo_4_wheel_run(timer, m1_dir_pin, m1_pwm_pin, m1_pwm);
    lo_4_wheel_run(timer, m2_dir_pin, m2_pwm_pin, m2_pwm);
    lo_4_wheel_run(timer, m3_dir_pin, m3_pwm_pin, m3_pwm);
    lo_4_wheel_run(timer, m4_dir_pin, m4_pwm_pin, m4_pwm);
    return 0;
}


int lo_4_wheel_calculation(int vx, int vy, int omega){
	m1_pwm = (vx+vy+omega);
	m2_pwm = (vx-vy-omega);
	m3_pwm = (-vx-vy+omega);
	m4_pwm = (-vx+vy-omega);

	// ---------- NORMALIZATION (CRTT) ----------
	float maxraw_1 = MAX(fabs(m1_pwm), fabs(m2_pwm));
	float maxraw_2 = MAX(fabs(m3_pwm), fabs(m4_pwm));
	float maxraw = MAX(maxraw_1, maxraw_2);

	if(maxraw > 255.0){
	float scale = 255.0 / maxraw;
	m1_pwm = (m1_pwm * scale);
	m2_pwm = (m2_pwm * scale);
	m3_pwm = (m3_pwm * scale);
	m4_pwm = (m4_pwm * scale);
	}
	// -----------------------------------------


	current = millis();
	if(current-previous >= 1000)
	{
//		printf("m1 = %f", m1_pwm);
//		printf("m2 = %f", m2_pwm);
//		printf("m3 = %f", m3_pwm);
//		printf("m4 = %f", m4_pwm);
		printf("m1 = %.2f  |  m2 = %.2f  |  m3 = %.2f |  m4 = %.2f\n", m1_pwm, m2_pwm, m3_pwm, m4_pwm);
		previous = millis();
	}

//	printf("m1 = %f  |  m2 = %f  |  m3 = %f |  m4 = %f", m1_pwm, m2_pwm, m3_pwm, m4_pwm);
	return 0;
}

void lo_4_wheel_run(TIM_HandleTypeDef *htim, uint16_t dir_pin, uint8_t mot_pin, float pwm){
	if(pwm > 0){
		HAL_GPIO_WritePin(GPIOD, dir_pin, SET);
	}else{
		HAL_GPIO_WritePin(GPIOD, dir_pin, RESET);
		pwm = abs(pwm);
	}
//	printf("pwm = %.2f\n", pwm);
	motor_set_speed255(htim, mot_pin, pwm);
}
