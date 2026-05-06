#ifndef INC_KFS_ARM_CONTROL_H_
#define INC_KFS_ARM_CONTROL_H_

#include "bot.h"

#define PIVOT_1_K			TIM_CHANNEL_1	//PA15
#define PIVOT_2_K			TIM_CHANNEL_4	//PB11
#define PIVOT_3_K			TIM_CHANNEL_2	//PA1

#define PIVOT_1_K_TIM_N		htim2	//TIM2 CH1
#define PIVOT_2_K_TIM_N		htim2	//TIM2 CH4
#define PIVOT_3_K_TIM_N		htim2	//TIM2 CH2

#define SPARK_PULSE_PIN		TIM_CHANNEL_1	//PE9	PB14

#define SPARK_PULSE_TIM_N	htim12	//TIM1 CH1

#define OFFSET_PWM_PIN		TIM_CHANNEL_2	//PE11 PE10
#define OFFSET_DIR_PIN		GPIO_PIN_7	//PE7
#define OFFSET_DIR_PORT		GPIOE

#define OFFSET_PWM_TIM_N	htim1	//TIM1 CH2

#define OFFSET_PWM_VAL		200

#define SPARK_UP_SPEED    	1850U
#define SPARK_STOP        	1500U
#define SPARK_DOWN_SPEED  	1150U

void kfs_arm_setup();
void kfs_arm_handler();

extern int t2_K_pos_1, t3_K_pos_1, t1_K_pos_1;
extern int t2_K_pos_2, t3_K_pos_2, t1_K_pos_2;

typedef struct {
  uint8_t kfs_s1_state;
  uint8_t kfs_s2_state;
  uint8_t kfs_s3_state;
} kfs_state_t;

extern kfs_state_t kfs_state;

#endif
