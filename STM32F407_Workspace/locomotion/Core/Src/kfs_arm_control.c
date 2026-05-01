#include "kfs_arm_control.h"

int t2_K_pos_1 = 80,  t3_K_pos_1 = 105,  t1_K_pos_1 = 33;
int t2_K_pos_2 = 100, t3_K_pos_2 = 85, t1_K_pos_2 = 97;

kfs_state_t kfs_state;

bool lastTriangle = false;
bool lastCross = false;


//Function definitions
void kfs_arm_setup(){
	kfs_state.kfs_s1_state = 0;
	kfs_state.kfs_s2_state = 0;
	kfs_state.kfs_s3_state = 0;

	Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K, t1_K_pos_1);
	HAL_Delay(50);
	Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K, t2_K_pos_1);
	HAL_Delay(50);
	Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K, t3_K_pos_1);
	HAL_Delay(50);
}


void kfs_arm_handler(){

	bool currentTriangle = btnStatus.triangle;

	if (currentTriangle && !lastTriangle) {
	  kfs_state.kfs_s1_state = !kfs_state.kfs_s1_state;

	  Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K,  kfs_state.kfs_s1_state ? t1_K_pos_2 : t1_K_pos_1);
	}
	lastTriangle = currentTriangle;
	//	printf("Current Triangle: %d\n", currentTriangle);
	//	HAL_Delay(1000);

	bool currentCross = btnStatus.cross;

	if (currentCross && !lastCross) {
	  kfs_state.kfs_s2_state = !kfs_state.kfs_s2_state;
	  kfs_state.kfs_s3_state = !kfs_state.kfs_s3_state;

	  Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K,  kfs_state.kfs_s2_state ? t2_K_pos_2 : t2_K_pos_1);

	  Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K,  kfs_state.kfs_s3_state ? t3_K_pos_2 : t3_K_pos_1);
	}
	lastCross = currentCross;

	//OFFSET CONTROL
	if (btnStatus.left) {
	HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, SET);
	motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);

	} else if (btnStatus.right) {
	HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, RESET);
	motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);

	} else {
	motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, 0); // STOP motor
	}

	//SPARK CONTROL
	if (btnStatus.up) {
	Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_UP_SPEED);
	} else if(btnStatus.down) {
	Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_DOWN_SPEED);
	}else {
	Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_STOP);
	}

}
