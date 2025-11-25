#ifndef __SYSTICK__
#define __SYSTICK__


#include "stm32f10x.h"                  // Device header

void systick_init(void);
uint32_t millis(void);
void delay_ms(uint32_t time);

#endif 