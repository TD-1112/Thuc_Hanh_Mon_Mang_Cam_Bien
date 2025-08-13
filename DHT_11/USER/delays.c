#include "delays.h"

void Timer2_Init(void)
{
	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	// Init timer
	TIM_TimeBaseInitTypeDef timerInit; 
	// mode up counter
	timerInit.TIM_CounterMode = TIM_CounterMode_Up;
	// period = 0xFFFF
	timerInit.TIM_Period = 0xFFFF;
	// prescaler = 72 - 1
	timerInit.TIM_Prescaler = 72 - 1;
	// clock division
	TIM_TimeBaseInit(TIM2, &timerInit);
	// Enable TIM2
	TIM_Cmd(TIM2, ENABLE);
}


void Delay1Ms(void)
{
	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < 1000) {
	}
}

void Delay_Ms(uint32_t u32DelayInMs)
{
	
	while (u32DelayInMs) {
		Delay1Ms();
		--u32DelayInMs;
	}
}