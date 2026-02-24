#ifndef	INC_STM32F407XX_H
#define INC_STM32F407XX_H

#include <stdint.h>
#include <stm32f407xx_usart.h>


#define SET					1
#define RESET				0


//Peripheral Base Addresses
//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*
//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$
#define RCC_BASE_ADDR								0x40023800UL

#define GPIOA_BASE_ADDR								0x40020000UL
#define GPIOD_BASE_ADDR								0x40020C00UL

#define USART1_BASE_ADDR							0x40011000UL
#define USART2_BASE_ADDR							0x40004400UL

//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$//$$
//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*





//Peripheral Register Typedef Struct
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????*/
typedef struct{
	volatile uint32_t RCC_CR;					//RCC clock control register (RCC_CR)
	volatile uint32_t RCC_PLLCFGR;				//RCC PLL configuration register (RCC_PLLCFGR)
	volatile uint32_t RCC_CFGR;					//RCC clock configuration register (RCC_CFGR)
	volatile uint32_t RCC_CIR;					//RCC clock interrupt register (RCC_CIR)
	volatile uint32_t RCC_AHB1RSTR;				//RCC AHB1 peripheral reset register (RCC_AHB1RSTR)
	volatile uint32_t RCC_AHB2RSTR;				//RCC AHB2 peripheral reset register (RCC_AHB2RSTR)
	volatile uint32_t RCC_AHB3RSTR;				//RCC AHB3 peripheral reset register (RCC_AHB3RSTR)
	volatile uint32_t RESERVED0;				//RESERVED
	volatile uint32_t RCC_APB1RSTR;				//RCC APB1 peripheral reset register (RCC_APB1RSTR)
	volatile uint32_t RCC_APB2RSTR;				//RCC APB2 peripheral reset register (RCC_APB2RSTR)
	volatile uint32_t RESERVED1;				//RESERVED
	volatile uint32_t RESERVED2;				//RESERVED
	volatile uint32_t RCC_AHB1ENR;				//RCC AHB1 peripheral clock enable register (RCC_AHB1ENR)
	volatile uint32_t RCC_AHB2ENR;				//RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)
	volatile uint32_t RCC_AHB3ENR;				//RCC AHB3 peripheral clock enable register (RCC_AHB3ENR)
	volatile uint32_t RESERVED3;				//RESERVED
	volatile uint32_t RCC_APB1ENR;				//RCC APB1 peripheral clock enable register (RCC_APB1ENR)
	volatile uint32_t RCC_APB2ENR;				//RCC APB2 peripheral clock enable register (RCC_APB2ENR)
	volatile uint32_t RESERVED4;				//RESERVED
	volatile uint32_t RESERVED5;				//RESERVED
	volatile uint32_t RCC_AHB1LPENR;			//RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR)
	volatile uint32_t RCC_AHB2LPENR;			//RCC AHB2 peripheral clock enable in low power mode register (RCC_AHB2LPENR)
	volatile uint32_t RCC_AHB3LPENR;			//RCC AHB3 peripheral clock enable in low power mode register (RCC_AHB3LPENR)
	volatile uint32_t RESERVED6;				//RESERVED
	volatile uint32_t RCC_APB1LPENR;			//RCC APB1 peripheral clock enable in low power mode register (RCC_APB1LPENR)
	volatile uint32_t RCC_APB2LPENR;			//RCC APB2 peripheral clock enabled in low power mode register (RCC_APB2LPENR)
	volatile uint32_t RESERVED7;				//RESERVED
	volatile uint32_t RESERVED8;				//RESERVED
	volatile uint32_t RCC_BDCR;					//RCC Backup domain control register (RCC_BDCR)
	volatile uint32_t RCC_CSR;					//RCC clock control & status register (RCC_CSR)
	volatile uint32_t RESERVED9;				//RESERVED
	volatile uint32_t RESERVED10;				//RESERVED
	volatile uint32_t RCC_SSCGR;				//RCC spread spectrum clock generation register (RCC_SSCGR)
	volatile uint32_t RCC_PLLI2SCFGR;			//RCC PLLI2S configuration register (RCC_PLLI2SCFGR)
}RCC_RegDef_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct {
	volatile uint32_t GPIOx_MODER;				//GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	volatile uint32_t GPIOx_OTYPER;				//GPIO port output type register (GPIOx_OTYPER)
	volatile uint32_t GPIOx_OSPEEDR;			//GPIO port output speed register (GPIOx_OSPEEDR)
	volatile uint32_t GPIOx_PUPDR;				//GPIO port pull-up/pull-down register (GPIOx_PUPDR)
	volatile uint32_t GPIOx_IDR;				//GPIO port input data register (GPIOx_IDR)
	volatile uint32_t GPIOx_ODR;				//GPIO port output data register (GPIOx_ODR)
	volatile uint32_t GPIOx_BSRR;				//GPIO port bit set/reset register (GPIOx_BSRR)
	volatile uint32_t GPIOx_LCKR;				//GPIO port configuration lock register (GPIOx_LCKR)
	volatile uint32_t GPIOx_AFRL;				//GPIO alternate function low register (GPIOx_AFRL)
	volatile uint32_t GPIOx_AFRH;				//GPIO alternate function high register (GPIOx_AFRH)
}GPIO_RegDef_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct USART_RegDef_t{
	volatile USART_SR_byte_t USART_SR;			//Status register (USART_SR)
	volatile uint32_t USART_DR;					//Data register (USART_DR)
	volatile USART_BRR_byte_t USART_BRR;		//Baud rate register (USART_BRR)
	volatile USART_CR1_byte_t USART_CR1;		//Control register 1 (USART_CR1)
	volatile USART_CR2_byte_t USART_CR2;		//Control register 2 (USART_CR2)
	volatile USART_CR3_byte_t USART_CR3;		//Control register 3 (USART_CR3)
	volatile USART_GTPR_byte_t USART_GTPR;		//Guard time and prescaler register (USART_GTPR)
}USART_RegDef_t;


//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????*/

#define RCC										((RCC_RegDef_t*)RCC_BASE_ADDR)

//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????*/






//Peripheral Address Definitions
//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&
/*________________________________________________________________________________________________________________________________________________________________________________*/


//GPIO ADDRESS DEFINITIONS
#define GPIOA								((GPIO_RegDef_t*) GPIOA_BASE_ADDR)
#define GPIOD								((GPIO_RegDef_t*) GPIOD_BASE_ADDR)

#define USART1								((USART_RegDef_t*) USART1_BASE_ADDR)
#define USART2								((USART_RegDef_t*) USART2_BASE_ADDR)








//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&//&&
/*________________________________________________________________________________________________________________________________________________________________________________*/

//PERIPHERAL CLOCK ENABLE MACRO DEFINITION
#define GPIOA_PCLK_EN() 				do { RCC->RCC_AHB1ENR |= (1 << 0); } while(0)
#define GPIOD_PCLK_EN() 				do { RCC->RCC_AHB1ENR |= (1 << 3); } while(0)

#define USART2_PCLK_EN()				do { RCC->RCC_APB1ENR |= (1 << 17); } while(0);


//PERIPHERAL CLOCK DISABLE MACRO DEFINITION
#define GPIOA_PCLK_DI() 				do { RCC->RCC_AHB1ENR &= ~(1 << 0); } while(0)
#define GPIOD_PCLK_DI() 				do { RCC->RCC_AHB1ENR &= ~(1 << 3); } while(0)

#define USART2_PCLK_DI()				do { RCC->RCC_APB1ENR &= ~(1 << 17); } while(0);












#endif
