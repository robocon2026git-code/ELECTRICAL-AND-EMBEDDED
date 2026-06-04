/* stm32f4xx_it.c
 * Interrupt Service Routines
 * Only the HAL_UART_RxCpltCallback is added in USER CODE sections.
 * All other handlers are unchanged from CubeMX generation.
 */

#include "main.h"
#include "stm32f4xx_it.h"

extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern UART_HandleTypeDef huart2;

void NMI_Handler(void)                { while(1); }
void HardFault_Handler(void)          { while(1); }
void MemManage_Handler(void)          { while(1); }
void BusFault_Handler(void)           { while(1); }
void UsageFault_Handler(void)         { while(1); }
void SVC_Handler(void)                {}
void DebugMon_Handler(void)           {}
void PendSV_Handler(void)             {}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart2);
}

void OTG_FS_IRQHandler(void) {
    HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}

/* USER CODE BEGIN 1 */
#include "user.h"

// Called automatically by HAL after each byte is received in IT mode.
// Routes to uart_rx_callback() which runs the state machine and re-arms.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uart_rx_callback(huart);
    }
}
/* USER CODE END 1 */
