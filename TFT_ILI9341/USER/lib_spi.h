#ifndef __LIB_SPI__
#define __LIB_SPI__

#include "stm32f10x.h"                  // Device header
#include "stdio.h"

//---------------------- SPI1 Pin Definition ----------------------//
#define SPI1_SCK_PORT      GPIOA
#define SPI1_SCK_PIN       GPIO_Pin_5

#define SPI1_MISO_PORT     GPIOA
#define SPI1_MISO_PIN      GPIO_Pin_6

#define SPI1_MOSI_PORT     GPIOA
#define SPI1_MOSI_PIN      GPIO_Pin_7

//---------------------- SPI2 Pin Definition ----------------------//
#define SPI2_SCK_PORT      GPIOB
#define SPI2_SCK_PIN       GPIO_Pin_13

#define SPI2_MISO_PORT     GPIOB
#define SPI2_MISO_PIN      GPIO_Pin_14

#define SPI2_MOSI_PORT     GPIOB
#define SPI2_MOSI_PIN      GPIO_Pin_15

//---------------------------------------------------------------//

typedef enum 
{
	MASTER,
	SLAVE
} spi_mode;


void spi_init(SPI_TypeDef *SPIx, spi_mode mode, uint32_t speed, GPIO_TypeDef *CS_Port, uint16_t CS_Pin);

uint8_t spi_write_byte(uint8_t byte);

#endif
