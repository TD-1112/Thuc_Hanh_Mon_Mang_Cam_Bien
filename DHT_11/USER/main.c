#include "stm32f10x.h"
#include "uart.h"
#include "delays.h"
#include "dht11.h"

void DHT11_Read(void);



int main(void)
{
	USART1_Init(9600);
	Timer2_Init();
	DHT11_Init();
	while (1) 
	{
		DHT11_Read();
		GPIOC->ODR ^= GPIO_Pin_13;
		Delay_Ms(1000);
	}
}

