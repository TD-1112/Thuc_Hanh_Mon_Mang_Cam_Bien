#include "lib_uart.h"
#include "fsm.h"

/*-----Private variable-----*/
uint8_t data_receive_1[DATA_SIZE] = {};
uint8_t data_receive_2[DATA_SIZE] = {};
uint8_t data_receive_3[DATA_SIZE] = {};

volatile char flag_receive = 0;
volatile uint8_t cnt_data = 0;
uint16_t rx_len = 0; // Store length from FSM

static USART_TypeDef *bus_base;

void usart_init(USART_TypeDef *bus, uint32_t baud_rate, pin _pin)
{
	bus_base = bus;

	GPIO_InitTypeDef GPIO;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	USART_InitTypeDef USART;

	if (bus == USART1)
	{
		if (_pin == DEFAULT)
		{

			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

			GPIO.GPIO_Pin = TX1_NOREMAP;
			GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(USART1_PORT_NOREMAP, &GPIO);

			GPIO.GPIO_Pin = RX1_NOREMAP;
			GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(USART1_PORT_NOREMAP, &GPIO);
		}
		else
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

			GPIO.GPIO_Pin = TX1_REMAP;
			GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(USART1_PORT_REMAP, &GPIO);

			GPIO.GPIO_Pin = RX1_REMAP;
			GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(USART1_PORT_REMAP, &GPIO);
		}
		NVIC_EnableIRQ(USART1_IRQn);
	}
	else if (bus == USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO.GPIO_Pin = TX2;
		GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(USART2_PORT, &GPIO);

		GPIO.GPIO_Pin = RX2;
		GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(USART2_PORT, &GPIO);

		NVIC_EnableIRQ(USART2_IRQn);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		GPIO.GPIO_Pin = TX3;
		GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(USART3_PORT, &GPIO);

		GPIO.GPIO_Pin = RX3;
		GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(USART3_PORT, &GPIO);

		NVIC_EnableIRQ(USART3_IRQn);
	}

	USART.USART_BaudRate = baud_rate;
	USART.USART_WordLength = USART_WordLength_8b;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART.USART_Parity = USART_Parity_No;

	USART_Init(bus, &USART);
	USART_Cmd(bus, ENABLE);

	USART_ITConfig(bus, USART_IT_RXNE, ENABLE);
}
/**
 * @brief send a single character via USART
 *
 * @param _chr
 */
void usart_send_char(char _chr)
{
	while (!(bus_base->SR & USART_SR_TXE))
	{
	}
	bus_base->DR = _chr;
}

/**
 * @brief send a string via USART1
 *
 * @param _str
 */
void usart_send_string(char *_str)
{
	while (*_str)
	{
		while (!(bus_base->SR & USART_SR_TXE))
			;
		bus_base->DR = *_str++;
	}
}

/**
 * @brief send a 16-bit unsigned integer via USART
 *
 * @param u16_number
 */
void usart_send_u16(uint16_t u16_number)
{
	char buffer[6];
	sprintf(buffer, "%d", u16_number);
	usart_send_string(&buffer);
}

/**
 * @brief send a byte in hexadecimal format via USART
 *
 * @param value
 */
void usart_send_hex(uint8_t value)
{
	char buffer[4];
	sprintf(buffer, "%02X", value);
	usart_send_string(buffer);
}

/**
 * @brief send a float number via USART
 *
 * @param f_number
 * @param decimal_places
 */
void usart_send_float(float f_number, uint8_t decimal_places)
{
	char buffer[32];
	char format[10];
	sprintf(format, "%%.%df", decimal_places);
	sprintf(buffer, format, f_number);
	usart_send_string(&buffer);
}
/**
 * @brief send and frame data via USART
 *
 * @param data
 * @param length_of_data
 */
void usart_send_data(uint8_t *data, uint16_t length_of_data)
{
	for (uint16_t i = 0; i < length_of_data; i++)
	{
		while (!(bus_base->SR & USART_SR_TXE))
		{
		}
		bus_base->DR = *data++;
	}
}

/**
 * @brief interrupt handler for USART
 *
 * @note This function is called when a character is received via USART1.
 * It reads the character from the data register and stores it in the data_receive array.
 * When a newline character is received, it sets the flag_receive variable to indicate that a complete message has been received.
 * The function also resets the cnt_data variable to prepare for the next message.
 */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t temp = USART_ReceiveData(USART1);
		fsm_get_message(temp, data_receive_1);
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		char temp = bus_base->DR;
		if (temp != '\n')
		{
			data_receive_2[cnt_data++] = temp;
		}
		else
		{
			data_receive_2[cnt_data] = 0;
			flag_receive = 1;
			cnt_data = 0;
		}
	}
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		char temp = bus_base->DR;
		if (temp != '\n')
		{
			data_receive_3[cnt_data++] = temp;
		}
		else
		{
			data_receive_3[cnt_data] = 0;
			flag_receive = 1;
			cnt_data = 0;
		}
	}
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

/**
 * @brief print the received data
 *
 */
void print_data(void)
{
	if (bus_base == USART1)
	{
		usart_send_string((char *)data_receive_1);
	}
	else if (bus_base == USART2)
	{
		usart_send_string((char *)data_receive_2);
	}
	else if (bus_base == USART3)
	{
		usart_send_string((char *)data_receive_3);
	}
}

/**
 * @brief check the state of the received data
 *
 * @return return 1 if data is received, 0 otherwise
 */
bool uart_is_message(void)
{
	// Use FSM's is_message function
	if (is_message(&rx_len))
	{
		return true;
	}
	return false;
}

UART_Handler UART;

/**
 * @brief Initialize UART Handler with function pointers
 * @note This function runs automatically before main() using __attribute__((constructor))
 */
void UART_FirstInit(void) __attribute__((constructor));

void UART_FirstInit(void)
{
	UART.init = usart_init;
	UART.send_char = usart_send_char;
	UART.send_string = usart_send_string;
	UART.send_data = usart_send_data;
	UART.send_u16 = usart_send_u16;
	UART.send_hex = usart_send_hex;
	UART.send_float = usart_send_float;
	UART.print_data = print_data;
	UART.uart_is_message = uart_is_message;
}