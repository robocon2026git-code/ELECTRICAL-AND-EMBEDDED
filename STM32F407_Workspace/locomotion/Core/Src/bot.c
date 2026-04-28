#include "bot.h"

BOT_Status_t war_status;

int system_status() {
	war_status.staff_p1 = 0;
	war_status.staff_p2 = 0;
	war_status.staff_p3 = 0;
	war_status.kfs_p1 = 0;
	war_status.kfs_p2 = 0;
	war_status.kfs_p3 = 0;
	war_status.kfs_ToF = 43;
	war_status.bot_speed = 0;

	return 0;
}


int odu() {
	  recieve_uart(&huart2);

	  lo_4_wheel_handler(&htim3);

	  kfs_arm_handler();

	  staff_arm_control();

//	  system_status();

//	  HAL_UART_Transmit(&huart2, (uint8_t*)&war_status, sizeof(war_status), 10);

	  return 0;
}


void bot_setup(){
	  printf("STM32 Ready\n");
	  kfs_arm_setup();
	  staff_arm_setup();

	  HAL_Delay(1000);
}
