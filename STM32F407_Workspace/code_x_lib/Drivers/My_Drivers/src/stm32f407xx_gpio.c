#include "stm32f407xx_gpio.h"

void GPIO_WriteToPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t val) {
	if (val == 1){
		pGPIOx->GPIOx_ODR |= (SET << GPIO_PIN);
	}else{
		pGPIOx->GPIOx_ODR &= ~(1 << GPIO_PIN);
	}
}
