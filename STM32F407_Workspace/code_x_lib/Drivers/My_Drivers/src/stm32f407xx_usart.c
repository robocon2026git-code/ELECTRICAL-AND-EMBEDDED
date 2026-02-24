#include "stm32f407xx.h"
#include "stm32f407xx_usart.h"


int USART_Status(USART_RegDef_t *pUSART) {
	return pUSART->USART_SR.usart_sr;
}

int USART_SetBaudRate(USART_RegDef_t *pUSART) {
	pUSART->USART_BRR.bits.DIV_Fraction = 10;
	return 0;
}

int USART_Init(USART_RegDef_t *pUSART) {
	//Enabling the USART ENABLE Register to start send and receive data
	pUSART->USART_CR1.bits.UE = ~1;
	pUSART->USART_CR1.bits.UE = SET;

	pUSART->USART_CR1.bits.TE &=
}
