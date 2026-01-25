/*
 * user.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Admin
 */


#include <user.h>


uint8_t rx_val;
Packet rx_pkt;
uint8_t ch, len;

float LX_usr;
float LY_usr;
float RX_usr;
float RY_usr;

BitfieldButtonStatusUsr btnStatus;


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
	// Use values directly
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

//	printf("FLAG = %02X | LX = %.2f | LY = %.2f | RX = %.2f | RY = %.2f\n", rx_pkt.btn_flag,  rx_pkt.lx, rx_pkt.ly, rx_pkt.rx, rx_pkt.ry);
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




void Servo_WriteAngle(TIM_HandleTypeDef *timer, uint8_t angle){
	//Clamp value 0-180
	if(angle > 180)angle=180;

	//Map 0-180 -> 1000 - 2000us
	uint16_t pulse = 1000 + (angle * 1000)/180;

	__HAL_TIM_SET_COMPARE(timer, TIM_CHANNEL_2, pulse);
}

