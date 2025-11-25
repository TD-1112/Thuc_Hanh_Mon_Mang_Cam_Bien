#include "tft_ili9341.h"

tft_dev _tft_dev;
uint16_t POINT_COLOR = BLACK;
uint16_t BACK_COLOR = WHITE;

static GPIO_TypeDef *TFT_CS_Port;
static uint16_t TFT_CS_Pin;
static GPIO_TypeDef *TFT_RS_Port;
static uint16_t TFT_RS_Pin;
static GPIO_TypeDef *TFT_RST_Port;
static uint16_t TFT_RST_Pin;

/*------------------ Send a single byte ------------------*/
void TFT_Send(TFT_Mode mode, uint8_t value)
{
    TFT_CS_CLR(TFT_CS_Port, TFT_CS_Pin);

    if (mode == TFT_CMD)
        TFT_RS_CLR(TFT_RS_Port, TFT_RS_Pin);
    else
        TFT_RS_SET(TFT_RS_Port, TFT_RS_Pin);

    spi_write_byte(value);

    TFT_CS_SET(TFT_CS_Port, TFT_CS_Pin);
}

/*------------------ Write a command followed by data ------------------*/
void TFT_WriteReg(uint8_t reg, uint8_t val)
{
    TFT_Send(TFT_CMD, reg);
    TFT_Send(TFT_DATA, val);
}

/*------------------ Configure control GPIO pins ------------------*/
static void TFT_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clocks for used GPIO ports (handle arbitrary ports, not only GPIOB)
    if (TFT_CS_Port == GPIOA || TFT_RS_Port == GPIOA || TFT_RST_Port == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (TFT_CS_Port == GPIOB || TFT_RS_Port == GPIOB || TFT_RST_Port == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (TFT_CS_Port == GPIOC || TFT_RS_Port == GPIOC || TFT_RST_Port == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (TFT_CS_Port == GPIOD || TFT_RS_Port == GPIOD || TFT_RST_Port == GPIOD)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (TFT_CS_Port == GPIOE || TFT_RS_Port == GPIOE || TFT_RST_Port == GPIOE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = TFT_CS_Pin;
    GPIO_Init(TFT_CS_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TFT_RS_Pin;
    GPIO_Init(TFT_RS_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TFT_RST_Pin;
    GPIO_Init(TFT_RST_Port, &GPIO_InitStructure);
}

/*------------------ Hardware reset of the TFT ------------------*/
static void TFT_Reset(void)
{
    TFT_RST_CLR(TFT_RST_Port, TFT_RST_Pin);
    delay_ms(100);
    TFT_RST_SET(TFT_RST_Port, TFT_RST_Pin);
    delay_ms(50);
}

/*------------------ Set display orientation ------------------*/
void TFT_Direction(uint8_t dir)
{
    _tft_dev.set_x_cmd = 0x2A;
    _tft_dev.set_y_cmd = 0x2B;
    _tft_dev.wram_cmd = 0x2C;

    switch (dir)
    {
    case 0:
        _tft_dev.width = LCD_W;
        _tft_dev.height = LCD_H;
        TFT_WriteReg(0x36, 0x08);
        break;
    case 1:
        _tft_dev.width = LCD_H;
        _tft_dev.height = LCD_W;
        TFT_WriteReg(0x36, 0x68);
        break;
    case 2:
        _tft_dev.width = LCD_W;
        _tft_dev.height = LCD_H;
        TFT_WriteReg(0x36, 0xC8);
        break;
    case 3:
    default:
        _tft_dev.width = LCD_H;
        _tft_dev.height = LCD_W;
        TFT_WriteReg(0x36, 0xA8);
        break;
    }
}

/*------------------ Initialize TFT (ILI9341) ------------------*/
void TFT_Init(gpio_config *cfg)
{
    // 1) Assign GPIO configuration
    TFT_CS_Port = cfg->CS_Port;
    TFT_CS_Pin = cfg->CS_Pin;
    TFT_RS_Port = cfg->RS_Port;
    TFT_RS_Pin = cfg->RS_Pin;
    TFT_RST_Port = cfg->RST_Port;
    TFT_RST_Pin = cfg->RST_Pin;

    // 2) Initialize SPI2 (Master, high speed)
    spi_init(SPI2, MASTER, SPI_BaudRatePrescaler_2, TFT_CS_Port, TFT_CS_Pin);

    // 3) Configure control GPIOs (CS/RS/RST)
    TFT_GPIO_Init();

    // 4) Reset the display
    TFT_Reset();

    // 5) Initialization command sequence (per ILI9341 reference)
    TFT_Send(TFT_CMD, 0xCF);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0xD9);
    TFT_Send(TFT_DATA, 0x30);

    TFT_Send(TFT_CMD, 0xED);
    TFT_Send(TFT_DATA, 0x64);
    TFT_Send(TFT_DATA, 0x03);
    TFT_Send(TFT_DATA, 0x12);
    TFT_Send(TFT_DATA, 0x81);

    TFT_Send(TFT_CMD, 0xE8);
    TFT_Send(TFT_DATA, 0x85);
    TFT_Send(TFT_DATA, 0x10);
    TFT_Send(TFT_DATA, 0x7A);

    TFT_Send(TFT_CMD, 0xCB);
    TFT_Send(TFT_DATA, 0x39);
    TFT_Send(TFT_DATA, 0x2C);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x34);
    TFT_Send(TFT_DATA, 0x02);

    TFT_Send(TFT_CMD, 0xF7);
    TFT_Send(TFT_DATA, 0x20);

    TFT_Send(TFT_CMD, 0xEA);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x00);

    TFT_WriteReg(0xC0, 0x1B); // Power control
    TFT_WriteReg(0xC1, 0x12); // Power control
    TFT_Send(TFT_CMD, 0xC5);
    TFT_Send(TFT_DATA, 0x26);
    TFT_Send(TFT_DATA, 0x26);
    TFT_WriteReg(0xC7, 0xB0);

    TFT_WriteReg(0x3A, 0x55); // 16bit/pixel

    TFT_Send(TFT_CMD, 0xB1);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x1A);

    TFT_Send(TFT_CMD, 0xB6);
    TFT_Send(TFT_DATA, 0x0A);
    TFT_Send(TFT_DATA, 0xA2);

    TFT_WriteReg(0xF2, 0x00);
    TFT_WriteReg(0x26, 0x01);

    // Gamma settings
    TFT_Send(TFT_CMD, 0xE0);
    uint8_t gamma_pos[] = {0x1F, 0x24, 0x24, 0x0D, 0x12, 0x09, 0x52, 0xB7, 0x3F, 0x0C, 0x15, 0x06, 0x0E, 0x08, 0x00};
    for (uint8_t i = 0; i < 15; i++)
        TFT_Send(TFT_DATA, gamma_pos[i]);

    TFT_Send(TFT_CMD, 0xE1);
    uint8_t gamma_neg[] = {0x00, 0x1B, 0x1B, 0x02, 0x0E, 0x06, 0x2E, 0x48, 0x3F, 0x03, 0x0A, 0x09, 0x31, 0x37, 0x1F};
    for (uint8_t i = 0; i < 15; i++)
        TFT_Send(TFT_DATA, gamma_neg[i]);

    // Address window defaults
    TFT_Send(TFT_CMD, 0x2B);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x01);
    TFT_Send(TFT_DATA, 0x3F);

    TFT_Send(TFT_CMD, 0x2A);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0x00);
    TFT_Send(TFT_DATA, 0xEF);

    TFT_Send(TFT_CMD, 0x11); // Sleep out
    delay_ms(120);
    TFT_Send(TFT_CMD, 0x29); // Display ON

    // 6) Set default orientation
    TFT_Direction(3);
}

/*------------------ Set write window (column/row addresses) ------------------*/
void TFT_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    TFT_Send(TFT_CMD, _tft_dev.set_x_cmd);
    TFT_Send(TFT_DATA, x1 >> 8);
    TFT_Send(TFT_DATA, x1 & 0xFF);
    TFT_Send(TFT_DATA, x2 >> 8);
    TFT_Send(TFT_DATA, x2 & 0xFF);

    TFT_Send(TFT_CMD, _tft_dev.set_y_cmd);
    TFT_Send(TFT_DATA, y1 >> 8);
    TFT_Send(TFT_DATA, y1 & 0xFF);
    TFT_Send(TFT_DATA, y2 >> 8);
    TFT_Send(TFT_DATA, y2 & 0xFF);

    TFT_Send(TFT_CMD, _tft_dev.wram_cmd);
}

/*------------------ Set cursor to a single pixel ------------------*/
void TFT_SetCursor(uint16_t x, uint16_t y)
{
    TFT_SetWindow(x, y, x, y);
}

///*------------------ Draw a single pixel ------------------*/
//void TFT_DrawPoint(uint16_t x, uint16_t y)
//{
//    TFT_SetCursor(x, y);
//    TFT_Send(TFT_DATA, POINT_COLOR >> 8);
//    TFT_Send(TFT_DATA, POINT_COLOR & 0xFF);
//}

/*------------------ Clear the screen with a solid color ------------------*/
void TFT_Clear(uint16_t color)
{
    uint32_t i;
    TFT_SetWindow(0, 0, _tft_dev.width - 1, _tft_dev.height - 1);

    TFT_CS_CLR(TFT_CS_Port, TFT_CS_Pin);
    TFT_RS_SET(TFT_RS_Port, TFT_RS_Pin);

    for (i = 0; i < (uint32_t)_tft_dev.width * _tft_dev.height; i++)
    {
        spi_write_byte(color >> 8);
        spi_write_byte(color & 0xFF);
    }

    TFT_CS_SET(TFT_CS_Port, TFT_CS_Pin);
}


void TFT_WriteData_16Bit(u16 Data)
{	
	TFT_CS_CLR(TFT_CS_Port, TFT_CS_Pin);
	TFT_RS_SET(TFT_RS_Port, TFT_RS_Pin); 
	spi_write_byte(Data>>8);
	spi_write_byte(Data);
	TFT_RS_SET(TFT_RS_Port, TFT_RS_Pin); 
}
