			#include "locomotion.h"

			float  m1_pwm, m2_pwm, m3_pwm, m4_pwm;

			// 🔥 GLOBAL SPEED LIMIT
			int locomotion_max_pwm = 200;
			int locomtion_rotation_pwm = 70;

			unsigned long current = 0, previous = 0;


			// ==============================
			// MAIN HANDLER
			// ==============================
			int lo_4_wheel_handler(TIM_HandleTypeDef *timer) {

				int x = LY_usr;   // forward/back
				int y = LX_usr;   // left/right

				// Deadzone
				if (fabsf(x) < 20) x = 0;
				if (fabsf(y) < 20) y = 0;

				int vx = 0, vy = 0, omega = 0;

				// ==============================
				// 🔥 ROTATION (HIGHEST PRIORITY)
				// ==============================
				if (btnStatus.l1) {
					vx = 0;
					vy = 0;
					omega = locomtion_rotation_pwm;    // rotate left
			//        printf("Rotate left\n");
				}
				else if (btnStatus.r1) {
					vx = 0;
					vy = 0;
					omega = -locomtion_rotation_pwm;   // rotate right
			//        printf("Rotate right\n");
				}
			//    else if (btnStatus.up) {
			//           vx = 100;
			//           vy = 0;
			//           omega = 0;   // rotate right
			//   //        printf("Rotate right\n");
			//       }
			//    else if (btnStatus.down) {
			//            vx = -100;
			//            vy = 0;
			//            omega = 0;   // rotate right
			//    //        printf("Rotate right\n");
			//        }
			//    else if (btnStatus.right) {
			//            vx = 0;
			//            vy = 100;-
			//            omega = 0;   // rotate right
			//    //        printf("Rotate right\n");
			//        }
			//    else if (btnStatus.left) {
			//            vx = 0;
			//            vy = -100;
			//            omega = 0;   // rotate right
			//    //        printf("Rotate right\n");
//					}
				else {
					// ==============================
					// NORMAL MOVEMENT
					// ==============================
					vx = (x * locomotion_max_pwm) / 127;   // forward/back
					vy = (y * locomotion_max_pwm) / 127;   // 🔥 FIXED (no minus)
					omega = 0;
				}

				if(is_staff_mode)
					lo_4_wheel_calculation_staff_mode(vx, vy, omega);
				else
					lo_4_wheel_calculation_kfs_mode(vx, vy, omega);

				// Run motors
				lo_4_wheel_run(timer, m1_dir_pin, m1_pwm_pin, m1_pwm, m1_ind_pin);
				lo_4_wheel_run(timer, m2_dir_pin, m2_pwm_pin, m2_pwm, m2_ind_pin);
				lo_4_wheel_run(timer, m3_dir_pin, m3_pwm_pin, m3_pwm, m3_ind_pin);
				lo_4_wheel_run(timer, m4_dir_pin, m4_pwm_pin, m4_pwm, m4_ind_pin);

			//    printf("M1 PWM :%.2f | M2 PWM :%.2f | M3 PWM :%.2f | M4 PWM :%.2f\n", m1_pwm, m2_pwm, m3_pwm, m4_pwm);

				return 0;
			}


			// ==============================
			// KINEMATICS (FINAL FIXED)
			// ==============================
			int lo_4_wheel_calculation_staff_mode(int vx, int vy, int omega){

				// Standard mecanum equations
				float t1 = (-vx - vy - omega);   // m1
				float t2 = (-vx + vy - omega);   // m2
				float t3 = (-vx - vy + omega);   // m3
				float t4 = (-vx + vy + omega);   // m4

				// 🔥 FINAL MOTOR POLARITY FIX
				m1_pwm =  t1;
				m2_pwm = -t2;
				m3_pwm =  t3;
				m4_pwm = -t4;

				// ==============================
				// NORMALIZATION
				// ==============================
				float maxraw_1 = MAX(fabs(m1_pwm), fabs(m2_pwm));
				float maxraw_2 = MAX(fabs(m3_pwm), fabs(m4_pwm));
				float maxraw   = MAX(maxraw_1, maxraw_2);


				if(maxraw > locomotion_max_pwm){
					float scale = locomotion_max_pwm / maxraw;
					m1_pwm *= scale;
					m2_pwm *= scale;
					m3_pwm *= scale;
					m4_pwm *= scale;
				}

				return 0;
			}


			int lo_4_wheel_calculation_kfs_mode(int vx, int vy, int omega){

				// Standard mecanum equations
				float t1 = (-vx - vy - omega);   // m1
				float t2 = (-vx + vy - omega);   // m2
				float t3 = (-vx - vy + omega);   // m3
				float t4 = (-vx + vy + omega);   // m4

				// 🔥 FINAL MOTOR POLARITY FIX
				m1_pwm =  t1;
				m2_pwm = -t2;
				m3_pwm =  t3;
				m4_pwm = -t4;

				// ==============================
				// NORMALIZATION
				// ==============================
				float maxraw_1 = MAX(fabs(m1_pwm), fabs(m2_pwm));
				float maxraw_2 = MAX(fabs(m3_pwm), fabs(m4_pwm));
				float maxraw   = MAX(maxraw_1, maxraw_2);


				if(maxraw > locomotion_max_pwm){
					float scale = locomotion_max_pwm / maxraw;
					m1_pwm *= scale;
					m2_pwm *= scale;
					m3_pwm *= scale;
					m4_pwm *= scale;
				}

				return 0;
			}

			// ==============================
			// MOTOR DRIVER
			// ==============================
			void lo_4_wheel_run(TIM_HandleTypeDef *htim,
								uint16_t dir_pin,
								uint8_t mot_pin,
								float pwm,
								uint16_t ind_pin){

				if(pwm > 0){
					HAL_GPIO_WritePin(GPIOC, dir_pin, SET);
					HAL_GPIO_WritePin(GPIOC, ind_pin, SET);
				} else {
					HAL_GPIO_WritePin(GPIOC, dir_pin, RESET);
					HAL_GPIO_WritePin(GPIOC, ind_pin, RESET);
					pwm = abs(pwm);
				}

				motor_set_speed255(htim, mot_pin, pwm);
			}
