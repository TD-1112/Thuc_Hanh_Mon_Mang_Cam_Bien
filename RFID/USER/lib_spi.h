#ifndef __LIB_SPI__
#define __LIB_SPI__

#include "stm32f10x.h"     // Device header
#include "stm32f10x_spi.h" // Keil::Device:StdPeriph Drivers:SPI
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC

void spi_config(void);
uint8_t spi_send_byte(uint8_t byte);

#endif
