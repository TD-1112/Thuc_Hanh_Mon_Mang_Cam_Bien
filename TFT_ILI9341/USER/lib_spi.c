#include "lib_spi.h"

static SPI_TypeDef *base_spi;
static GPIO_TypeDef *cs_port = NULL;
static uint16_t cs_pin = 0;

void spi_init(SPI_TypeDef *SPIx, spi_mode mode, uint32_t speed, GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
	base_spi = SPIx;
	cs_port = CS_Port;
	cs_pin = CS_Pin;
	
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	if (SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);
		
		// SCK, MISO, MOSI
		GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN | SPI1_MISO_PIN | SPI1_MOSI_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(SPI1_SCK_PORT, &GPIO_InitStructure);
	}
	else if (SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		// SCK, MISO, MOSI
		GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MISO_PIN | SPI2_MOSI_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(SPI2_SCK_PORT, &GPIO_InitStructure);
	}
	else
	{
		return; 
	}

	if (CS_Port != NULL && CS_Pin != 0)
	{
		GPIO_InitStructure.GPIO_Pin = CS_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(CS_Port, &GPIO_InitStructure);
		GPIO_SetBits(CS_Port, CS_Pin); 
	}

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_BaudRatePrescaler = speed;
	
	SPI_InitStructure.SPI_Mode = (mode == MASTER) ? SPI_Mode_Master : SPI_Mode_Slave;

	SPI_Init(SPIx, &SPI_InitStructure);
	
	SPI_Cmd(SPIx, ENABLE);
}


uint8_t spi_write_byte(uint8_t byte)
{
	while ((base_spi->SR & SPI_I2S_FLAG_TXE) == RESET);
	base_spi->DR = byte;
	while ((base_spi->SR & SPI_I2S_FLAG_RXNE) == RESET);
	return base_spi->DR;
}
