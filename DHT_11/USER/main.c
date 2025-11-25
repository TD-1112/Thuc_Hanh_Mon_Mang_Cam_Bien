#include "stm32f10x.h"
#include "lib_dht11.h"
#include "lib_timer2.h"
#include "lib_uart.h"

value_dht11 _value;

int main(void)

{
	usart_init(USART1, 115200, DEFAULT);
	Timer2_Init();
	DHT11_Init(GPIOA, GPIO_Pin_7);
	usart_send_string("oke\n");
	while (1)
	{
		if (DHT11_Read(&_value))
		{
			usart_send_string("Temp :");
			usart_send_u16((uint16_t)_value.temp);
			usart_send_char('\n');
			usart_send_string("Humi :");
			usart_send_u16((uint16_t)_value.humi);

			usart_send_string("\nTemp_f :");
			usart_send_float(_value.temp_f , 2);
			usart_send_char('\n');
			usart_send_string("Humi_f :");
			usart_send_float(_value.humi_f,2);
			usart_send_char('\n');
		}
		else
		{
			usart_send_string("DHT11 Read ERROR\n");
		}
		Delay_ms(1000);
	}
}
