#ifndef __LIB_GPIO__
#define __LIB_GPIO__

#include "stm32f10x.h" // Device header

#define LED_PIN GPIO_Pin_13
#define LED_PORT GPIOC

void gpio_init(void);
void on(void);
void off(void);

#endif
