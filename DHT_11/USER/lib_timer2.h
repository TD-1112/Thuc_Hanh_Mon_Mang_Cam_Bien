#ifndef __LIB_TIMER2__
#define __LIB_TIMER2__

#include "stm32f10x.h"                  // Device header

void Timer2_Init(void);
void Delay_us(uint16_t us);
void Delay_ms(uint32_t ms);
uint32_t millis(void);
void TIM2_IRQHandler(void);


#endif
