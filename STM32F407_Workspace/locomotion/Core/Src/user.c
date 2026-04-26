/*
 * user.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */
/////
#include <user.h>


uint8_t rx_val;
Packet rx_pkt;
uint8_t ch, len;

float LX_usr;
float LY_usr;
float RX_usr;
float RY_usr;
float L2_usr;
float R2_usr;

BitfieldButtonStatusUsr btnStatus;




//This function can be used for get SysTick timer value
uint32_t millis(void){
	return HAL_GetTick();
}




void recieve_uart(UART_HandleTypeDef *uart){
	while(1){
		 do {
			  HAL_UART_Receive(uart, &ch, 1, HAL_MAX_DELAY);
		 }while (ch != STX);

		// Read length
		HAL_UART_Receive(uart, &len, 1, HAL_MAX_DELAY);
		if (len != sizeof(Packet)) {
			 continue;
	}

		// Read payload directly into struct
		HAL_UART_Receive(uart, (uint8_t*)&rx_pkt, len, HAL_MAX_DELAY);
		break;
	}
	parse_uart_data();
}


void parse_uart_data() {
	// Use values directly
	if (rx_pkt.btn_flag & (1 << 9)) {
		printf("R1 pressed\n");
		btnStatus.r1 = 1;
	}
	if (rx_pkt.btn_flag & (1 << 8)) {
		printf("L1 pressed\n");
		btnStatus.l1 = 1;
	}
	if (rx_pkt.btn_flag & (1 << 7)) {
		printf("Circle pressed\n");
		btnStatus.circle = 1;
	}
	if (rx_pkt.btn_flag & (1 << 6)) {
		printf("Square pressed\n");
		btnStatus.square = 1;
	}
	if (rx_pkt.btn_flag & (1 << 5)) {
		printf("Cross pressed\n");
		btnStatus.cross = 1;
	}
	if (rx_pkt.btn_flag & (1 << 4)) {
		printf("Triangle pressed\n");
		btnStatus.triangle = 1;
	}
	if (rx_pkt.btn_flag & (1 << 3)) {
		printf("Right pressed\n");
		btnStatus.right = 1;
	}
	if (rx_pkt.btn_flag & (1 << 2)) {
		printf("Left pressed\n");
		btnStatus.left = 1;
	}
	if (rx_pkt.btn_flag & (1 << 1)) {
		printf("Down pressed\n");
		btnStatus.down = 1;
	}
	if (rx_pkt.btn_flag & (1 << 0)) {
		printf("Up pressed\n");
		btnStatus.up = 1;
	}

	LX_usr = rx_pkt.lx;
	LY_usr = rx_pkt.ly;
	RX_usr = rx_pkt.rx;
	RY_usr = rx_pkt.ry;
	L2_usr = rx_pkt.l2;
	R2_usr = rx_pkt.r2;

//	printf("FLAG = %02X | LX = %.2f | LY = %.2f | RX = %.2f | RY = %.2f\n", rx_pkt.btn_flag,  rx_pkt.lx, rx_pkt.ly, rx_pkt.rx, rx_pkt.ry);
	printf("FLAG = %02X | LX = %.2f | LY = %.2f | RX = %.2f | RY = %.2f | L2 = %.2f | R2 = %.2f\n", rx_pkt.btn_flag,  LX_usr, LY_usr, RX_usr, RY_usr, L2_usr, R2_usr);
}


//Speed value 0.0 <--> 1.0
void motor_set_speed(TIM_HandleTypeDef *htim, uint32_t channel, float speed)
{
    // speed: 0.0 → 1.0
    if (speed < 0.0f) speed = 0.0f;
    if (speed > 1.0f) speed = 1.0f;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t ccr = (uint32_t)((arr + 1) * speed);

    __HAL_TIM_SET_COMPARE(htim, channel, ccr);
}


//This function allows to do analogwrite() like behavior val = (0 - 255)
void motor_set_speed255(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t val)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t ccr = (val * (arr + 1)) / 255;

    __HAL_TIM_SET_COMPARE(htim, channel, ccr);
}





//To write servo angle
void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t channel, uint8_t angle){
	//Clamp value 0-180
	if(angle > 180)angle=180;

	//Map 0-180 -> 1000 - 2000us
	uint16_t pulse = 1000 + (angle * 1000)/180;

	__HAL_TIM_SET_COMPARE(timer, channel, pulse);
}

void Bldc_writePulse(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t pulse) {
	if((pulse < 1000) || (pulse > 2000)) {
		return;
	}

	__HAL_TIM_SET_COMPARE(timer, channel, pulse);
}



//This function is used to map for BLDC motors
int bldc_maping(int val, int stop, int max_fw, int max_rw){
	if(val == 0){
		return stop;
	}
	if(val < 0){
		return map(val, -127, -1, max_rw, stop);
	}
	if(val > 0){
		return map(val, 1, 127, stop, max_fw);
	}
	return 0;
}


//Arduino like function used to map values
long map(long val, long in_min, long in_max, long out_min, long out_max) {
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




void Stepper_SetDirection(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Stepper_Dir_t dir) {
    if(dir == CW) {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    }
}



void Stepper_SetSpeed(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t hz) {
    if (hz < 10) hz = 10;

    // Calculate ARR based on 1MHz timer clock (PSC=83)
    uint32_t new_arr = (1000000 / hz) - 1;

    // Update Timer registers using the pointer
    __HAL_TIM_SET_AUTORELOAD(htim, new_arr);

    // Keep 50% Duty Cycle (new_arr / 2)
    __HAL_TIM_SET_COMPARE(htim, channel, new_arr / 2);

    /* IMPORTANT: If you change ARR while the timer is running,
       you should force an update or ensure 'Preload' is enabled
       in CubeMX to prevent a glitchy "long pulse".
    */
}


