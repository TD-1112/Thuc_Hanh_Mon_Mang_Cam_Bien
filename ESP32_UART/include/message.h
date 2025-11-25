#ifndef __MESSAGE__
#define __MESSAGE__

#include <stdint.h>
#include "math.h"

typedef struct
{
    uint16_t start_message;  // start frame is 0xAA55 uint16_t
    uint8_t type_message;
    uint16_t length_message; // length of message 
    uint8_t data[100];
    uint16_t check_sum;
} Frame_Message;

/**
 * tại sao lại dùng struct bởi vì khi mà cấp phát bộ nhớ cho struct tương tự như mảng 1 chiều
 * 
 */

 /**
  * @brief enum Type_Message
  * 
  */
typedef enum
{
    ASK_MESSAGE = 0x00,
    RESPONSE_MESSAGE = 0x01
} Type_Message; // kiểu dữ liệu 

uint16_t create_message(Type_Message type_mess, uint16_t value, uint8_t *data_out);
uint16_t caculate_checksum(uint8_t *data, uint16_t length_data);

#endif