#ifndef INC_STM32F407xx_GPIO_H
#define INC_STM32F407xx_GPIO_H

#include "stm32f407xx.h"


//GPIO PIN Definition
#define GPIO_PIN_0					0
#define GPIO_PIN_1					1
#define GPIO_PIN_2					2
#define GPIO_PIN_3					3
#define GPIO_PIN_4					4
#define GPIO_PIN_5					5
#define GPIO_PIN_6					6
#define GPIO_PIN_7					7
#define GPIO_PIN_8					8
#define GPIO_PIN_9					9
#define GPIO_PIN_10					10
#define GPIO_PIN_11					11
#define GPIO_PIN_12					12
#define GPIO_PIN_13					13
#define GPIO_PIN_14					14
#define GPIO_PIN_15					15



void GPIO_WriteToPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t val);



#endif
