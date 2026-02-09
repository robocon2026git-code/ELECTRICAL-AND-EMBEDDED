#ifndef INC_STM32F407xx_USART_H
#define INC_STM32F407xx_USART_H

//#include <stm32f407xx.h>
#include <stdint.h>


//USART STATUS REGISTER BIT-FIELDS
typedef union {
	struct {
		volatile uint32_t PE			: 1;
		volatile uint32_t FE			: 1;
		volatile uint32_t NF			: 1;
		volatile uint32_t ORE			: 1;
		volatile uint32_t IDLE			: 1;
		volatile uint32_t RXNE			: 1;
		volatile uint32_t TC			: 1;
		volatile uint32_t TXE			: 1;
		volatile uint32_t LBD			: 1;
		volatile uint32_t CTS			: 1;
		volatile uint32_t RESERVED		: 22;
	}bits;
	uint32_t usart_sr;
}USART_SR_byte_t;


//USART BAUD RATE REGISTER (USART_BRR)
typedef union {
	struct {
		volatile uint32_t DIV_Fraction	: 4;
		volatile uint32_t DIV_Mantisa 	: 12;
		volatile uint32_t RESERVED0	 	: 16;
	}bits;
	uint32_t usart_brr;
}USART_BRR_byte_t;


//USART CONTROL REGISTER1 BIT-FIELDS
typedef union {
	struct {
		volatile uint32_t SBK			: 1;
		volatile uint32_t RWU			: 1;
		volatile uint32_t RE			: 1;
		volatile uint32_t TE			: 1;
		volatile uint32_t IDLEIE		: 1;
		volatile uint32_t RXNEIE		: 1;
		volatile uint32_t TCIE			: 1;
		volatile uint32_t TXEIE			: 1;
		volatile uint32_t PEIE			: 1;
		volatile uint32_t PS			: 1;
		volatile uint32_t PCE			: 1;
		volatile uint32_t WAKE			: 1;
		volatile uint32_t M				: 1;
		volatile uint32_t UE			: 1;
		volatile uint32_t RESERVED0		: 1;
		volatile uint32_t OVER8			: 1;
		volatile uint32_t RESERVED1		: 16;
	}bits;
	uint32_t usart_cr1;
}USART_CR1_byte_t;


//USART CONTROL REGISTER2 BIT-FIELDS
typedef union {
	struct {
		volatile uint32_t ADD			: 4;
		volatile uint32_t RESERVED0		: 1;
		volatile uint32_t LBDL			: 1;
		volatile uint32_t LBDIE			: 1;
		volatile uint32_t RESERVED1		: 1;
		volatile uint32_t LBCL			: 1;
		volatile uint32_t CPHA			: 1;
		volatile uint32_t CPOL			: 1;
		volatile uint32_t CLKEN			: 1;
		volatile uint32_t STOP			: 2;
		volatile uint32_t LINEN			: 1;
		volatile uint32_t RESERVED2		: 17;
	}bits;
	uint32_t usart_cr2;
}USART_CR2_byte_t;


//USART CONTROL REGISTER3 BIT-FIELDS
typedef union {
	struct {
		volatile uint32_t EIE			: 1;
		volatile uint32_t IREN			: 1;
		volatile uint32_t IRLP			: 1;
		volatile uint32_t HDSEL			: 1;
		volatile uint32_t NACK			: 1;
		volatile uint32_t SCEN			: 1;
		volatile uint32_t DMAR			: 1;
		volatile uint32_t DMAT			: 1;
		volatile uint32_t RTSE			: 1;
		volatile uint32_t CTSE			: 1;
		volatile uint32_t CTSIE			: 1;
		volatile uint32_t ONEBIT		: 1;
		volatile uint32_t RESERVED0		: 20;
	}bits;
	uint32_t usart_cr3;
}USART_CR3_byte_t;


//USART GUARD TIME AND PRESCALER REGISTER (USART_GTPR)
typedef union {
	struct {
		volatile uint32_t PSC			: 8;
		volatile uint32_t GT			: 8;
		volatile uint32_t RESERVED0		: 16;
	}bits;
	uint32_t usart_gtpr;
}USART_GTPR_byte_t;


typedef struct USART_RegDef_t USART_RegDef_t;

USART_Status(USART_RegDef_t *pUSART);
int USART_SetBaudRate(USART_RegDef_t *pUSART);



#endif



