#include "tim2.h"
#include "uart.h"
#include "bh1750.h"
#include "lib_message.h"
#include "lib_convert.h"

void GPIOA_Init(void);
void Init(void);
void Loop(void);

int main(void)
{
	Init();
  while (1)
  {
	Loop();
  }
}


void GPIOA_Init(void)
{
  RCC->APB2ENR |= (1 << 2) | ( 1 << 0);
  AFIO->MAPR = ( 0x02 << 24);
  GPIOA->CRH &= ~(0xF << 28);
  GPIOA->CRH |= (0x3 << 28);
}

void Init(void)
{
	GPIOA_Init(); 
  USART1_Init(9600);
  Timer2_Init();
  BH1750_Init();
	
}

void Loop(void)
{
	float value = BH1750_ReadLux();
	uint8_t data[20];
	uint8_t length = Message_Create_Frame_Respond_LUX(PORT_A, value, data);
	USART1_Send_Data(data, length);
	GPIOA->ODR ^= (1 << 15);
	Delay_ms(1000);
	
}