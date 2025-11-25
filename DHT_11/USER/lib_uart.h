#ifndef __LIB_UART__
#define __LIB_UART__

#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdbool.h>

#define USART1_PORT_NOREMAP 				GPIOA
#define TX1_NOREMAP 								GPIO_Pin_9 // define cac variable 
#define RX1_NOREMAP 								GPIO_Pin_10
/*---------------------------------------------*/
#define USART1_PORT_REMAP 					GPIOB
#define TX1_REMAP 									GPIO_Pin_6
#define RX1_REMAP   								GPIO_Pin_7
/*---------------------------------------------*/
#define USART2_PORT 								GPIOA
#define TX2													GPIO_Pin_2
#define RX2													GPIO_Pin_3
/*---------------------------------------------*/
#define USART3_PORT 								GPIOB
#define TX3 												GPIO_Pin_10
#define RX3													GPIO_Pin_11
/*---------------------------------------------*/


#define DATA_SIZE 200

// truyen du lieu giua 2 con vdk (esp, pi, ....) -> uart -> FSM 

typedef enum
{
    DEFAULT = 0, 
		REMAP        
} pin;

void usart_init(USART_TypeDef *bus,uint32_t baud_rate , pin _pin);
void usart_send_char(char _chr);
void usart_send_string(char *_str);
void usart_send_data(uint8_t *data, uint16_t length_of_data);
void usart_send_u16(uint16_t u16_number);
void usart_send_float(float f_number,uint8_t decimal_places);
void print_data(void);
bool is_message(void);

#endif 
