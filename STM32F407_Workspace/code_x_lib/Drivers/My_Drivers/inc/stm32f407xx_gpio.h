#ifndef INC_STM32F407xx_GPIO_H
#define INC_STM32F407xx_GPIO_H

#include "stm32f407xx.h"


//GPIO PIN Definition
#define GPIO_PIN_0							0
#define GPIO_PIN_1							1
#define GPIO_PIN_2							2
#define GPIO_PIN_3							3
#define GPIO_PIN_4							4
#define GPIO_PIN_5							5
#define GPIO_PIN_6							6
#define GPIO_PIN_7							7
#define GPIO_PIN_8							8
#define GPIO_PIN_9							9
#define GPIO_PIN_10							10
#define GPIO_PIN_11							11
#define GPIO_PIN_12							12
#define GPIO_PIN_13							13
#define GPIO_PIN_14							14
#define GPIO_PIN_15							15


//GPIO MODES
#define GPIO_INPUT							0
#define GPIO_OUTPUT							1
#define GPIO_ALTERNATE_FUNCTION				2
#define GPIO_ANALOG							3


//GPIO ALTERNATE FUNCTIONS
#define AF0									0
#define AF1									1
#define AF2									2
#define AF3									3
#define AF4									4
#define AF5									5
#define AF6									6
#define AF7									7
#define AF8									8
#define AF9									9
#define AF10								10
#define AF11								11
#define AF12								12
#define AF13								13
#define AF14								14
#define AF15								15



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GPIO FUNCTION PROTOTYPES
void GPIO_WriteToPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t val);
void GPIO_ModeSel(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint8_t mode);
void GPIO_SelectAlternateFunction(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PIN, uint16_t AF_Mode);


static inline void GPIO_SetPin(GPIO_RegDef_t *pGPIO, uint8_t pin) {
	pGPIO->GPIOx_BSRR = (1 << pin);
}

static inline void GPIO_ResetPin(GPIO_RegDef_t *pGPIO, uint8_t pin) {
	pGPIO->GPIOx_BSRR = (1 << (pin + 16U));
}


#endif
