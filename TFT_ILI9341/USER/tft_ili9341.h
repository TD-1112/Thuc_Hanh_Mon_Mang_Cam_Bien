#ifndef __TFT_ILI9341_H
#define __TFT_ILI9341_H

#include "stm32f10x.h"
#include "lib_spi.h"
#include "SysTick.h"
#include "stdlib.h"

//----------------------TFT Config----------------------//
#define USE_HORIZONTAL  0
#define LCD_W 240
#define LCD_H 320

//---------------------- color ----------------------//
#define WHITE       0xFFFF
#define BLACK       0x0000	  
#define BLUE        0x001F  
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define MAGENTA     0xF81F
#define GRAY        0x8430
#define BROWN       0XBC40
#define BRRED       0XFC07

typedef enum {
    TFT_CMD = 0,
    TFT_DATA
} TFT_Mode;

//---------------------- Macro ----------------------//
#define TFT_CS_CLR(PORT, PIN)   GPIO_ResetBits((PORT), (PIN))
#define TFT_CS_SET(PORT, PIN)   GPIO_SetBits((PORT), (PIN))

#define TFT_RS_CLR(PORT, PIN)   GPIO_ResetBits((PORT), (PIN))
#define TFT_RS_SET(PORT, PIN)   GPIO_SetBits((PORT), (PIN))

#define TFT_RST_CLR(PORT, PIN)  GPIO_ResetBits((PORT), (PIN))
#define TFT_RST_SET(PORT, PIN)  GPIO_SetBits((PORT), (PIN))

//---------------------- default pin----------------------//
#define TFT_CS_PORT   GPIOB
#define TFT_CS_PIN    GPIO_Pin_9
#define TFT_RS_PORT   GPIOB
#define TFT_RS_PIN    GPIO_Pin_8
#define TFT_RST_PORT  GPIOB
#define TFT_RST_PIN   GPIO_Pin_12

//---------------------- struct ----------------------//
typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t id;
	uint8_t	dir;
	uint16_t wram_cmd;
	uint16_t set_x_cmd;
	uint16_t set_y_cmd;
} tft_dev;

typedef struct
{
	GPIO_TypeDef *CS_Port;
	uint16_t CS_Pin;

	GPIO_TypeDef *RS_Port;
	uint16_t RS_Pin;

	GPIO_TypeDef *RST_Port;
	uint16_t RST_Pin;
	
} gpio_config;

//---------------------- global variable ----------------------//
extern tft_dev _tft_dev;
extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

//---------------------- Prototype ----------------------//
void TFT_Init(gpio_config *cfg);

void TFT_Send(TFT_Mode mode, uint8_t value);
void TFT_WriteReg(uint8_t reg, uint8_t val);
void TFT_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_SetCursor(uint16_t x, uint16_t y);
void TFT_Clear(uint16_t color);
void TFT_Direction(uint8_t dir);
void TFT_WriteData_16Bit(u16 Data);


#endif
