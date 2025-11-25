#include "lib_gpio.h"

void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

void on(void)
{
    LED_PORT->ODR &= ~LED_PIN;
}

void off(void)
{
    LED_PORT->ODR |= LED_PIN;
}
