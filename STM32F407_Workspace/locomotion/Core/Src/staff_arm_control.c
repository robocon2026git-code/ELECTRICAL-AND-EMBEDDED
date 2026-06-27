
/*
 * staff_arm_control.c
 *
 * Stepper : TIM4 CH3 (PB8)
 *           1/8 microstep → 1600 steps/rev
 *           14:1 gear ratio
 *           180° max = 11200 steps
 *           Speed: 1500 Hz
 *
 * Servo P2: TIM9 CH1 (PE5) — Up/Down with R1
 * Servo P3: TIM9 CH2 (PE6) — Left/Right with R1
 *
 * Button Mapping:
 *   UP   + L1  → Stepper CW
 *   DOWN + L1  → Stepper CCW
 *   UP   + R1  → Servo P2 increase angle
 *   DOWN + R1  → Servo P2 decrease angle
 *   LEFT + R1  → Servo P3 increase angle
 *   RIGHT+ R1  → Servo P3 decrease angle
 *   CIRCLE     → Pneumatic PD0=ON,  PD1=OFF
 *   SQUARE     → Pneumatic PD0=OFF, PD1=ON
 */

#include "staff_arm_control.h"

// --- Globals ---
int     current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
int     current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
int32_t target_steps_1   = 0;
int32_t current_steps_1  = 0;

#define STEPPER_DOCK     	-1500
#define STEPPER_INITIAL		0
#define STEPPER_TAKE   		2075
#define STEPPER_DROP        2085

int SERVO_ALIGN = 0;

#define SERVO_INITIAL		128
#define SERVO_TAKE  		42
#define SERVO_DOCK  		128


int32_t STEPPER_ALIGN = 0;
bool dockServoDelayActive = false;
bool dockServoMoved = false;
bool dockAlignMode = false;

bool last_circle = false;
bool pneumatic_state = false;

long dockStartPosition = 0;

static bool     pwm_running     = false;

//int STEPPER_INITIAL = 0;



// -----------------------------------------------------------------------
void staff_arm_setup(void) {
    current_angle_p2 = STAFF_ARM_P2_INITIAL_ANGLE;
    current_angle_p3 = STAFF_ARM_P3_INITIAL_ANGLE;
    target_steps_1   = 0;
    current_steps_1  = 0;

    // Init Servo P2
    Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
    HAL_TIM_PWM_Start(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE);
    HAL_Delay(150);

    // Init Servo P3
    Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);
    HAL_TIM_PWM_Start(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE);
    HAL_Delay(150);
}

// -----------------------------------------------------------------------
void staff_arm_control(void) {

//    manual_staff_arm_control();
	auto_staff_arm_control();

    Pnuematic_OnOff();
}


void manual_staff_arm_control() {
    // ----------------------------------------------------------------
    // 1. STEPPER — runs while button held, stops instantly on release
    // ----------------------------------------------------------------
	uint32_t now = HAL_GetTick();

    static uint32_t last_servo_time = 0;
    static uint32_t last_step_tick  = 0;
    static bool     last_dir_cw     = true;

    bool move_ccw  = (btnStatus.triangle);
    bool move_cw = (btnStatus.cross);

   if (move_cw || move_ccw) {

        // Check if direction changed
        bool dir_changed = (move_cw != last_dir_cw);

        // Stop PWM briefly on direction change so driver latches new DIR
        if (dir_changed && pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
            HAL_Delay(1);
        }

        // Set direction BEFORE starting PWM
        if (move_cw) {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CW);
            last_dir_cw = true;
        } else {
            Stepper_SetDirection(STAFF_ARM_P1_DIR_PORT, STAFF_ARM_P1_DIR_PIN, CCW);
            last_dir_cw = false;
        }

        // Start PWM only if not already running
        if (!pwm_running) {
            Stepper_SetSpeed(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE, STAFF_ARM_STEPPER_SPEED_HZ);
            HAL_TIM_PWM_Start(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = true;
        }



        // Software position tracking at 1ms intervals
        if (now - last_step_tick >= 1) {
            last_step_tick = now;

            if (move_cw) {
                if (current_steps_1 < STAFF_ARM_STEP_LIMIT) {
                    current_steps_1++;
                } else {
                    // CW physical limit reached — stop
                    HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
                    pwm_running = false;
                }

            } else {
                if (current_steps_1 > -STAFF_ARM_STEP_LIMIT) {
                    current_steps_1--;
                } else {
                    // CCW physical limit reached — stop
                    HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
                    pwm_running = false;
                }

            }
        }

    } else {
        // Button released — stop immediately, hold position
        if (pwm_running) {
            HAL_TIM_PWM_Stop(&STAFF_ARM_P1_TIM_N, STAFF_ARM_P1_PULSE);
            pwm_running = false;
        }
        target_steps_1 = current_steps_1;
    }


   // ----------------------------------------------------------------
   // 2. SERVO CONTROL — every 20ms
   // ----------------------------------------------------------------
    if (now - last_servo_time >= 20) {
        last_servo_time = now;

        // P2 Up/Down
        if (btnStatus.down) current_angle_p2 += STAFF_ARM_P2_STEP_ANGLE;
        if (btnStatus.up) current_angle_p2 -= STAFF_ARM_P2_STEP_ANGLE;

        // P3 Left/Right
        if (btnStatus.left) current_angle_p3 += STAFF_ARM_P3_STEP_ANGLE;
        if (btnStatus.right) current_angle_p3 -= STAFF_ARM_P3_STEP_ANGLE;

        // Clamp P2
        if (current_angle_p2 > STAFF_ARM_P2_MAX_ANGLE) current_angle_p2 = STAFF_ARM_P2_MAX_ANGLE;
        if (current_angle_p2 < STAFF_ARM_P2_MIN_ANGLE) current_angle_p2 = STAFF_ARM_P2_MIN_ANGLE;

        // Clamp P3
        if (current_angle_p3 > STAFF_ARM_P3_MAX_ANGLE) current_angle_p3 = STAFF_ARM_P3_MAX_ANGLE;
        if (current_angle_p3 < STAFF_ARM_P3_MIN_ANGLE) current_angle_p3 = STAFF_ARM_P3_MIN_ANGLE;

        Servo_WriteAngle(&STAFF_ARM_P2_TIM_N, STAFF_ARM_P2_PULSE, (uint8_t)current_angle_p2);
        Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, (uint8_t)current_angle_p3);

    }
}


void auto_staff_arm_control() {
	uint32_t now = HAL_GetTick();

    static bool last_down = false;
    static bool last_cross = false;
    static bool last_triangle = false;
    static bool last_square = false;
    static bool last_up = false;
    static bool align_state = false;
    static uint32_t last_step_tick  = 0;
//    static uint32_t last_servo_tick  = 0;
    static bool returnServoPending = false;


    if(btnStatus.triangle && !last_triangle)
    {
        align_state = !align_state;

        if(align_state)
        {
            STEPPER_ALIGN = STEPPER_TAKE;
            SERVO_ALIGN   = SERVO_TAKE;
        }
        else
        {
            STEPPER_ALIGN = STEPPER_DOCK;
            SERVO_ALIGN   = SERVO_DOCK;
            returnServoPending = true;
        }
    }

    last_triangle = btnStatus.triangle;

    if(btnStatus.down && !last_down)
    {
        STEPPER_ALIGN -= 50;
//        Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TI    M_N, STAFF_ARM_P3_PULSE, SERVO_DOCK);
    }

    if(btnStatus.up && !last_up)
    {
        STEPPER_ALIGN += 50;
    }

    if(btnStatus.square && !last_square)
	 {
		 STEPPER_ALIGN = STEPPER_INITIAL;
	     SERVO_ALIGN = SERVO_INITIAL;
	    	}
    if(btnStatus.cross && !last_cross)
    {
        STEPPER_ALIGN = STEPPER_DROP;
        SERVO_ALIGN   = SERVO_DOCK;
    }
//    static bool crossPressed = false;
//    static bool servoMoved = false;
//    static uint32_t crossTime = 0;

//    if(btnStatus.cross && !crossPressed)
//    {
//        crossPressed = true;
//        servoMoved = false;
//        crossTime = HAL_GetTick();
//
//        STEPPER_ALIGN = STEPPER_DROP;
//    }
//
//    if(crossPressed)
//    {
//        if((HAL_GetTick() - crossTime) >= 500 && !servoMoved)
//        {
//            Servo_WriteAngle_168Mhz(
//                &STAFF_ARM_P3_TIM_N,
//                STAFF_ARM_P3_PULSE,
//                SERVO_DOCK);
//
//            servoMoved = true;
//        }
//    }
//
//    if(!btnStatus.cross)
//    {
//        crossPressed = false;
//    }
//
          last_up = btnStatus.up;
          last_down = btnStatus.down;
	      last_triangle = btnStatus.triangle;
	      last_square = btnStatus.square;
	      last_cross = btnStatus.cross;

	    // --------------------------------------------------
	    // Auto Move To Position
	    // --------------------------------------------------

	    if(current_steps_1 < STEPPER_ALIGN)
	    {
	        Stepper_SetDirection(
	            STAFF_ARM_P1_DIR_PORT,
	            STAFF_ARM_P1_DIR_PIN,
	            CCW);

	        if(!pwm_running)
	        {
	            Stepper_SetSpeed(
	                &STAFF_ARM_P1_TIM_N,
	                STAFF_ARM_P1_PULSE,
	                STAFF_ARM_STEPPER_SPEED_HZ);

	            HAL_TIM_PWM_Start(
	                &STAFF_ARM_P1_TIM_N,
	                STAFF_ARM_P1_PULSE);

	            pwm_running = true;
	        }
	    }
	    else if(current_steps_1 > STEPPER_ALIGN)
	    {
	        Stepper_SetDirection(
	            STAFF_ARM_P1_DIR_PORT,
	            STAFF_ARM_P1_DIR_PIN,
	            CW);

	        if(!pwm_running)
	        {
	            Stepper_SetSpeed(
	                &STAFF_ARM_P1_TIM_N,
	                STAFF_ARM_P1_PULSE,
	                STAFF_ARM_STEPPER_SPEED_HZ);

	            HAL_TIM_PWM_Start(
	                &STAFF_ARM_P1_TIM_N,
	                STAFF_ARM_P1_PULSE);

	            pwm_running = true;
	        }
	    }
	    else
	    {
	        if(pwm_running)
	        {
	            HAL_TIM_PWM_Stop(
	                &STAFF_ARM_P1_TIM_N,
	                STAFF_ARM_P1_PULSE);

	            pwm_running = false;
	        }
	    }

	    if(now - last_step_tick >= 1)
	    {
	        last_step_tick = now;

	        if(pwm_running)
	        {
	            if(current_steps_1 < STEPPER_ALIGN) {
	                current_steps_1++;

	                if(current_steps_1 > 1000)
	                	   {
	                			Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE,SERVO_TAKE);

	                	   }
//	        	    if(now - last_servo_tick >= 5000)
//	        	    {
//	        	        Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, SERVO_ALIGN);
//
//
//	        	        last_servo_tick = now;
//	        	    }
	            }

	            else if(current_steps_1 > STEPPER_ALIGN) {
	                current_steps_1--;
	                if(returnServoPending &&
	                      current_steps_1 < 3000)
	                   {
	                       Servo_WriteAngle_168Mhz(
	                           &STAFF_ARM_P3_TIM_N,
	                           STAFF_ARM_P3_PULSE,
	                           SERVO_DOCK);

	                       returnServoPending = false;
	                   }
	               }
//	        	    if(now - last_servo_tick >= 5000)
//	        	    {
//	        	        Servo_WriteAngle_168Mhz(&STAFF_ARM_P3_TIM_N, STAFF_ARM_P3_PULSE, SERVO_ALIGN);
//
//
//	        	        last_servo_tick = now;
//	        	    }
	            }
	        }
	    }

void Pneumatic_UpdateOutput(void)
{
    if(pneumatic_state)
    {
        HAL_GPIO_WritePin(PNEUMATIC_PORT,
                          PNEUMATIC_PIN_1,
                          GPIO_PIN_SET);

        HAL_GPIO_WritePin(PNEUMATIC_PORT,
                          PNEUMATIC_PIN_2,
                          GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(PNEUMATIC_PORT,
                          PNEUMATIC_PIN_1,
                          GPIO_PIN_RESET);

        HAL_GPIO_WritePin(PNEUMATIC_PORT,
                          PNEUMATIC_PIN_2,
                          GPIO_PIN_SET);
    }
}

// -----------------------------------------------------------------------
void Pnuematic_OnOff(void) {
//    static bool last_circle = false;
//    static bool pneumatic_state = false;  // false = OFF, true = ON

//    bool current_circle = (btnStatus.circle == 1);
//    bool lastOptions;
//
//    // Toggle only on button press (rising edge)
//    if (current_circle && !last_circle) {
//        pneumatic_state = !pneumatic_state;
//        if(currentOptions && !lastOptions)
//			{
//				is_staff_mode = !is_staff_mode;
//
//				last_circle = btnStatus.circle;
//
//				Pneumatic_UpdateOutput();
//			}
//
//                Pneumatic_UpdateOutput();
//            }
//
//            last_circle = current_circle;
//        }

	static uint8_t last_circle = 0;

	if(btnStatus.circle && !last_circle)
	{
	    pneumatic_state = !pneumatic_state;
	}

	last_circle = btnStatus.circle;

	if (pneumatic_state)
	{
	    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_RESET);
	}
	else
	{
	    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_1, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(PNEUMATIC_PORT, PNEUMATIC_PIN_2, GPIO_PIN_SET);
	}
//	int val = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0);
//	printf("VALUE = %d/n", val);
}
