#include "lib_spi.h"
#include "SysTick.h"
#include "gui.h"
#include "image.h"
#include "tft_ili9341.h"


int main()
{
    systick_init();
    gpio_config cfg;
    cfg.CS_Port = GPIOB;
    cfg.CS_Pin = GPIO_Pin_9;
    cfg.RS_Port = GPIOB;
    cfg.RS_Pin = GPIO_Pin_8;
    cfg.RST_Port = GPIOB;
    cfg.RST_Pin = GPIO_Pin_12;

    TFT_Init(&cfg);
    //    uint16_t colors[] = {
    //        RED, GREEN, BLUE,
    //        WHITE, BLACK, YELLOW,
    //        MAGENTA, CYAN, GRAY, BROWN, BRRED};

    //    uint8_t color_count = sizeof(colors) / sizeof(colors[0]);
    //
    //

    //    while (1)
    //    {
    //        for (uint8_t i = 0; i < color_count; i++)
    //        {
    //            TFT_Clear(colors[i]);
    //        }
    //    }
    Gui_DrawbmpUser(0, 60, 320, 240, gImage_keyboard);
}
