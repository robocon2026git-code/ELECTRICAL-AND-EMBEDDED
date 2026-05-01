/*
 * staff_arm_control.h
 */

#ifndef INC_STAFF_ARM_CONTROL_H_
#define INC_STAFF_ARM_CONTROL_H_

#include "bot.h"

// --- Servo Initial Angles ---
#define STAFF_ARM_P2_INITIAL_ANGLE      49U
#define STAFF_ARM_P3_INITIAL_ANGLE      90U

// --- Servo Angle Limits ---
#define STAFF_ARM_P2_MIN_ANGLE          10
#define STAFF_ARM_P2_MAX_ANGLE          170
#define STAFF_ARM_P3_MIN_ANGLE          0
#define STAFF_ARM_P3_MAX_ANGLE          180

// --- Servo Step Per Button Press ---
#define STAFF_ARM_P2_STEP_ANGLE         1
#define STAFF_ARM_P3_STEP_ANGLE         1

// --- Pneumatic Pins ---
#define PNEUMATIC_PORT                  GPIOD
#define PNEUMATIC_PIN_1                 GPIO_PIN_0
#define PNEUMATIC_PIN_2                 GPIO_PIN_1

// --- Stepper Motor (P1) ---
#define STAFF_ARM_P1_PULSE              TIM_CHANNEL_3   // PB8
#define STAFF_ARM_P1_TIM_N              htim4

#define STAFF_ARM_P1_DIR_PIN            GPIO_PIN_12     // PE12
#define STAFF_ARM_P1_DIR_PORT           GPIOE

// -----------------------------------------------------------------------
// Stepper Limit Calculation
// -----------------------------------------------------------------------
// Microstep : 1/8  → 1600 steps/rev
//             Driver DIP: SW1=OFF, SW2=ON, SW3=OFF
//
// Gear ratio : 14:1
// Max travel : 180°
//
// Limit = (steps_per_rev × gear_ratio × angle_deg) / 360
//       = (1600 × 14 × 180) / 360
//       = 11200 steps
// -----------------------------------------------------------------------
#define STAFF_ARM_STEPS_PER_REV         1600    // 1/8 microstep
#define STAFF_ARM_GEAR_RATIO            14
#define STAFF_ARM_MAX_ANGLE_DEG         180

#define STAFF_ARM_STEP_LIMIT \
    ((STAFF_ARM_STEPS_PER_REV * STAFF_ARM_GEAR_RATIO * STAFF_ARM_MAX_ANGLE_DEG) / 360)
// Result = 11200

// --- Stepper Speed ---
// 1500 Hz = smooth at 1/8 microstep
// Increase to 3000 Hz for faster movement
#define STAFF_ARM_STEPPER_SPEED_HZ      1500

// --- Servo Timers ---
#define STAFF_ARM_P2_PULSE              TIM_CHANNEL_1   // PE5
#define STAFF_ARM_P2_TIM_N              htim9

#define STAFF_ARM_P3_PULSE              TIM_CHANNEL_2   // PE6
#define STAFF_ARM_P3_TIM_N              htim9

// --- Function Declarations ---
void staff_arm_setup(void);
void staff_arm_control(void);
void Pnuematic_OnOff(void);

// --- Globals ---
extern int32_t target_steps_1;
extern int32_t current_steps_1;

#endif /* INC_STAFF_ARM_CONTROL_H_ */
