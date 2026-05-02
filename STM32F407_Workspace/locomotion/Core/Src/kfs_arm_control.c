#include "kfs_arm_control.h"

int t3_K_pos_1 = 173,  t1_K_pos_1 = 30, t2_K_pos_1 = 65;
int t3_K_pos_2 = 145, t1_K_pos_2 = 97, t2_K_pos_2 = 90;

kfs_state_t kfs_state;
bool lastTriangle = false;
bool lastCross = false;

void kfs_arm_setup() {
    kfs_state.kfs_s1_state = 0;
    kfs_state.kfs_s2_state = 0;
    kfs_state.kfs_s3_state = 0;

    // Set neutral (safe stop)
    Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, 1500);
//    HAL_Delay(3000);

    // No long delay needed for Spark MAX

    Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K, t1_K_pos_1);
    Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K, t2_K_pos_1);
    Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K, t3_K_pos_1);
}

void kfs_arm_handler(){
    // Triangle Toggle
    bool currentTriangle = btnStatus.triangle;
    if (currentTriangle && !lastTriangle) {
        kfs_state.kfs_s1_state = !kfs_state.kfs_s1_state;
        Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K, kfs_state.kfs_s1_state ? t1_K_pos_2 : t1_K_pos_1);
    }
    lastTriangle = currentTriangle;

    // Cross Toggle
    bool currentCross = btnStatus.cross;
    if (currentCross && !lastCross) {
        kfs_state.kfs_s2_state = !kfs_state.kfs_s2_state;
        kfs_state.kfs_s3_state = !kfs_state.kfs_s3_state;
        Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K, kfs_state.kfs_s2_state ? t2_K_pos_2 : t2_K_pos_1);
        Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K, kfs_state.kfs_s3_state ? t3_K_pos_2 : t3_K_pos_1);
    }
    lastCross = currentCross;

    // OFFSET MOTOR CONTROL
    if (btnStatus.left) {
        HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, GPIO_PIN_SET);
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    } else if (btnStatus.right) {
        HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, GPIO_PIN_RESET);
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    } else {
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, 0);
    }

    // SPARK (BLDC) CONTROL
    if (btnStatus.up) {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_UP_SPEED);
    } else if (btnStatus.down) {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_DOWN_SPEED);
    } else {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_STOP);
    }
}
