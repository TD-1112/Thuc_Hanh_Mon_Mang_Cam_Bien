#include "gui.h"
#include "font.h"

uint8_t TFT_Font_Direction = TFT_FONT_DIRECTION_HORIZONTAL;

/*----------------------------------- Draw a single pixel -----------------------------------*/
void TFT_DrawPoint(uint16_t x, uint16_t y)
{
    if (x >= _tft_dev.width || y >= _tft_dev.height)
        return;
    TFT_SetCursor(x, y);
    TFT_Send(TFT_DATA, POINT_COLOR >> 8);
    TFT_Send(TFT_DATA, POINT_COLOR & 0xFF);
}

/*----------------------------------- Draw a straight line -----------------------------------*/
void TFT_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1)
    {
        TFT_DrawPoint(x1, y1);
        if (x1 == x2 && y1 == y2)
            break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

/*----------------------------------- Draw rectangle outline -----------------------------------*/
void TFT_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    TFT_DrawLine(x1, y1, x2, y1);
    TFT_DrawLine(x1, y1, x1, y2);
    TFT_DrawLine(x1, y2, x2, y2);
    TFT_DrawLine(x2, y1, x2, y2);
}

/*----------------------------------- Fill a rectangle -----------------------------------*/
void TFT_FillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t i, j;
    for (i = y1; i <= y2; i++)
    {
        for (j = x1; j <= x2; j++)
        {
            POINT_COLOR = color;
            TFT_DrawPoint(j, i);
        }
    }
}

/*----------------------------------- Draw a circle outline -----------------------------------*/
void TFT_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
    int a, b;
    int di = 3 - (r << 1);
    a = 0; b = r;
    while (a <= b)
    {
        TFT_DrawPoint(x0 + a, y0 - b);
        TFT_DrawPoint(x0 + b, y0 - a);
        TFT_DrawPoint(x0 + b, y0 + a);
        TFT_DrawPoint(x0 + a, y0 + b);
        TFT_DrawPoint(x0 - a, y0 + b);
        TFT_DrawPoint(x0 - b, y0 + a);
        TFT_DrawPoint(x0 - b, y0 - a);
        TFT_DrawPoint(x0 - a, y0 - b);
        a++;
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/*----------------------------------- Fill a circle -----------------------------------*/
void TFT_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int a, b;
    int di = 3 - (r << 1);
    a = 0; b = r;
    while (a <= b)
    {
        uint16_t y_top = y0 - b;
        uint16_t y_bottom = y0 + b;
        for (uint16_t x = x0 - a; x <= x0 + a; x++)
        {
            POINT_COLOR = color;
            TFT_DrawPoint(x, y_top);
            TFT_DrawPoint(x, y_bottom);
        }
        for (uint16_t x = x0 - b; x <= x0 + b; x++)
        {
            POINT_COLOR = color;
            TFT_DrawPoint(x, y0 - a);
            TFT_DrawPoint(x, y0 + a);
        }

        a++;
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/*----------------------------------- Display a single ASCII character -----------------------------------*/
void TFT_ShowChar(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';
    for (t = 0; t < csize; t++)
    {
        temp = asc2_1608[chr][t];
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                POINT_COLOR = mode ? BACK_COLOR : POINT_COLOR;
                TFT_DrawPoint(x, y);
            }
            else if (mode)
            {
                POINT_COLOR = BACK_COLOR;
                TFT_DrawPoint(x, y);
            }
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/*----------------------------------- Display string -----------------------------------*/
void TFT_ShowString(uint16_t x, uint16_t y, const char *p, uint8_t size, uint8_t mode)
{
    while (*p != '\0')
    {
        TFT_ShowChar(x, y, *p, size, mode);
        x += size / 2;
        p++;
    }
}

/*----------------------------------- Display integer number -----------------------------------*/
void TFT_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / (uint32_t)pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                TFT_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else
                enshow = 1;
        }
        TFT_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}

/*----------------------------------- Display floating-point number -----------------------------------*/
void TFT_ShowFloat(uint16_t x, uint16_t y, float num, uint8_t len, uint8_t dec, uint8_t size)
{
    uint32_t int_part = (uint32_t)num;
    float frac = num - int_part;
    TFT_ShowNum(x, y, int_part, len, size);
    x += len * size / 2;
    TFT_ShowChar(x, y, '.', size, 0);
    x += size / 2;
    for (uint8_t i = 0; i < dec; i++)
    {
        frac *= 10;
        uint8_t digit = (uint8_t)frac;
        TFT_ShowChar(x, y, digit + '0', size, 0);
        x += size / 2;
        frac -= digit;
    }
}

/*----------------------------------- Draw raw image -----------------------------------*/
void Gui_DrawbmpUser(u16 x,u16 y, u16 picxelX, u16 picxelY ,const unsigned char *p) 
{
  int i; 
	unsigned char picH,picL; 
	TFT_SetWindow(x,y,x+picxelX-1,y+picxelY-1);
	for(i=0;i<picxelX*picxelY	;i++) 
	{	
	 	picL=*(p+i*2);	
		picH=*(p+i*2+1);				
		TFT_WriteData_16Bit(picH<<8|picL);  						
	}	
	TFT_SetWindow(0,0,_tft_dev.width-1,_tft_dev.height-1);
}
