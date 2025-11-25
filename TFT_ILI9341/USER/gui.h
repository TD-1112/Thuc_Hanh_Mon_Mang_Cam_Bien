#ifndef __GUI__
#define __GUI__

#include "stm32f10x.h"
#include "tft_ili9341.h"
#include "stdlib.h"
#include "math.h"


/* Point structure */
typedef struct
{
	uint16_t x;
	uint16_t y;
} POINT_T;

/* Size structure */
typedef struct
{
	uint16_t width;
	uint16_t height;
} SIZE_T;

/* Pixel size */
#define DOT_PIXEL_1X 1
#define DOT_PIXEL_2X 2
#define DOT_PIXEL_3X 3

/* Font direction */
#define TFT_FONT_DIRECTION_HORIZONTAL 0
#define TFT_FONT_DIRECTION_VERTICAL 1

extern uint8_t TFT_Font_Direction;

/* ========== Function Prototypes ========== */
void TFT_DrawPoint(uint16_t x, uint16_t y);
void TFT_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_FillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void TFT_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r);
void TFT_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

void TFT_ShowChar(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode);
void TFT_ShowString(uint16_t x, uint16_t y, const char *p, uint8_t size, uint8_t mode);
void TFT_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);
void TFT_ShowFloat(uint16_t x, uint16_t y, float num, uint8_t len, uint8_t dec, uint8_t size);

void Gui_DrawbmpUser(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char *bmp);

#endif
