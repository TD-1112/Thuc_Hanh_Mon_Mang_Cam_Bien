#ifndef __TIM2_H__
#define __TIM2_H__

#include "defines.h"

void Timer2_Init(void);
void Delay_us(uint16_t us);
void Delay_ms(uint32_t ms);
uint32_t millis(void);
void TIM2_IRQHandler(void);

#endif
