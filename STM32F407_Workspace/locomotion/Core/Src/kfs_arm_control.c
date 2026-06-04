#include "kfs_arm_control.h"

extern uint32_t millis(void);

// -------- SERVO SMOOTH CONTROL --------
uint32_t lastServoUpdate = 0;

int servo1_currentPos = 0;
int servo1_targetPos  = 0;

uint8_t servo1_moving = 0;

// -------- POSITION VALUES --------
int t3_K_pos_1 = 128,  t1_K_pos_1 = 20, t2_K_pos_1 = 120;
int t3_K_pos_2 = 146,  t1_K_pos_2 = 141, t2_K_pos_2 = 97;


// -------- STATE --------
kfs_state_t kfs_state;
bool lastTriangle = false;
bool lastCross = false;

// -------- SETUP --------
void kfs_arm_setup() {
    kfs_state.kfs_s1_state = 0;
    kfs_state.kfs_s2_state = 0;
    kfs_state.kfs_s3_state = 0;

    // Initialize servo positions
    servo1_currentPos = t1_K_pos_1;
    servo1_targetPos  = t1_K_pos_1;

    Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K, t1_K_pos_1);
    Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K, t2_K_pos_1);
    Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K, t3_K_pos_1);
}

// -------- MAIN HANDLER --------
void kfs_arm_handler()
{
    // -------- TRIANGLE → Smooth Servo 1 --------
    bool currentTriangle = btnStatus.triangle;

    if (currentTriangle && !lastTriangle)
    {
        // Toggle state
        kfs_state.kfs_s1_state = !kfs_state.kfs_s1_state;

        // Set new target position
        servo1_targetPos = kfs_state.kfs_s1_state ? t1_K_pos_2 : t1_K_pos_1;

        // Start movement
        servo1_moving = 1;
    }
    lastTriangle = currentTriangle;

    // -------- NON-BLOCKING SERVO UPDATE --------
    if (servo1_moving)
    {
        if (millis() - lastServoUpdate >= 10)   // speed control
        {
            lastServoUpdate = millis();

            if (servo1_currentPos < servo1_targetPos) {
                servo1_currentPos++;
            }
            else if (servo1_currentPos > servo1_targetPos) {
                servo1_currentPos--;
            }

            Servo_WriteAngle(&PIVOT_1_K_TIM_N, PIVOT_1_K, servo1_currentPos);

            if (servo1_currentPos == servo1_targetPos) {
                servo1_moving = 0;
            }
        }
    }

    // -------- CROSS → Servo 2 & 3 --------
    bool currentCross = btnStatus.cross;

    if (currentCross && !lastCross)
    {
        kfs_state.kfs_s2_state = !kfs_state.kfs_s2_state;
        kfs_state.kfs_s3_state = !kfs_state.kfs_s3_state;

        Servo_WriteAngle(&PIVOT_2_K_TIM_N, PIVOT_2_K,
                         kfs_state.kfs_s2_state ? t2_K_pos_2 : t2_K_pos_1);

        Servo_WriteAngle(&PIVOT_3_K_TIM_N, PIVOT_3_K,
                         kfs_state.kfs_s3_state ? t3_K_pos_2 : t3_K_pos_1);
    }
    lastCross = currentCross;

    // -------- OFFSET MOTOR --------
    if (btnStatus.left) {
        HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, GPIO_PIN_SET);
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    }
    else if (btnStatus.right) {
        HAL_GPIO_WritePin(OFFSET_DIR_PORT, OFFSET_DIR_PIN, GPIO_PIN_RESET);
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, OFFSET_PWM_VAL);
    }
    else {
        motor_set_speed255(&OFFSET_PWM_TIM_N, OFFSET_PWM_PIN, 0);
    }

    // -------- SPARK MAX (BLDC) --------
    if (btnStatus.up) {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_UP_SPEED);
    }
    else if (btnStatus.down) {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_DOWN_SPEED);
    }
    else {
        Bldc_writePulse(&SPARK_PULSE_TIM_N, SPARK_PULSE_PIN, SPARK_STOP);
    }
}
