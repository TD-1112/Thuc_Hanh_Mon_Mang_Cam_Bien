#ifndef delays_h
#define delays_h

#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO


void Timer2_Init(void);
void Delay1Ms(void);
void Delay_Ms(uint32_t u32DelayInMs);

#endif
