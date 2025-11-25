#ifndef __LIB_UART__
#define __LIB_UART__

#include "stm32f10x.h" // Device header
#include <string.h>
#include <stdbool.h>

#define USART1_PORT_NOREMAP GPIOA
#define TX1_NOREMAP GPIO_Pin_9
#define RX1_NOREMAP GPIO_Pin_10
/*---------------------------------------------*/
#define USART1_PORT_REMAP GPIOB
#define TX1_REMAP GPIO_Pin_6
#define RX1_REMAP GPIO_Pin_7
/*---------------------------------------------*/
#define USART2_PORT GPIOA
#define TX2 GPIO_Pin_2
#define RX2 GPIO_Pin_3
/*---------------------------------------------*/
#define USART3_PORT GPIOB
#define TX3 GPIO_Pin_10
#define RX3 GPIO_Pin_11
/*---------------------------------------------*/

#define DATA_SIZE 200

typedef enum
{
  DEFAULT = 0,
  REMAP
} pin;

extern uint8_t data_receive_1[DATA_SIZE];
extern volatile uint8_t cnt_data;
extern volatile char flag_receive;
extern uint16_t rx_len;

/* UART Handler Structure with function pointers (Application Layer) */
typedef struct
{
  // Data members
  USART_TypeDef *bus;

  // Function pointers for application-level operations
  void (*init)(USART_TypeDef *bus, uint32_t baud_rate, pin _pin);
  void (*send_char)(char _chr);
  void (*send_string)(char *_str);
  void (*send_data)(uint8_t *data, uint16_t length);
  void (*send_u16)(uint16_t u16_number);
  void (*send_hex)(uint8_t value);
  void (*send_float)(float f_number, uint8_t decimal_places);
  void (*print_data)(void);
  bool (*uart_is_message)(void);
} UART_Handler;

extern UART_Handler UART;

// Core functions (backward compatibility)
void usart_init(USART_TypeDef *bus, uint32_t baud_rate, pin _pin);
void usart_send_char(char _chr);
void usart_send_string(char *_str);
void usart_send_data(uint8_t *data, uint16_t length_of_data);
void usart_send_u16(uint16_t u16_number);
void usart_send_hex(uint8_t value);
void usart_send_float(float f_number, uint8_t decimal_places);
void print_data(void);
bool uart_is_message(void);

// UART Handler (OOP-style interface) - auto-initialized

#endif
