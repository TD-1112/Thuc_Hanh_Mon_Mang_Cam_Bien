#include "defines.h"

int main(void)
{
    Timer2_Init();
    USART1_Init(9600);
    BH1750_Init();
		USART1_Send_String("OKE\r\n");
    while (1)
    {
      float value = BH1750_ReadLux();
      USART1_Send_String("LUX: ");
      USART1_Send_Float(value);
      USART1_Send_String("\r\n");
      Delay_ms(1000);
    }
}


