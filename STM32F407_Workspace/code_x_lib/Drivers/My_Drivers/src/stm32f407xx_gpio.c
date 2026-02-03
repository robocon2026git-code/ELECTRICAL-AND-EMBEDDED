#include "stm32f407xx_gpio.h"


void GPIO_ModeSel(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t mode){
	pGPIOx->GPIOx_MODER &= ~(1 << (GPIO_PIN*2));
	pGPIOx->GPIOx_MODER |= (mode << (GPIO_PIN*2));
}


void GPIO_WriteToPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t val) {
	if (val == 1){
		pGPIOx->GPIOx_MODER &= ~(1 << (GPIO_PIN*2));			//Clearing bit to ensure the right value is writing
		pGPIOx->GPIOx_MODER |= (GPIO_OUTPUT << (GPIO_PIN*2));	//SETTING OUTPUT MODE
		pGPIOx->GPIOx_ODR |= (SET << GPIO_PIN);					//Writing value 1(SET)
	}else{
		pGPIOx->GPIOx_ODR &= ~(1 << GPIO_PIN);					//Writing value 0(RESET)
	}
}
