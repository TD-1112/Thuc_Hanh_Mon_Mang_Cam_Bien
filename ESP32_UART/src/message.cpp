#include "message.h"

/*
    khung bản tin (base)
    start 	| type 		| length | data   |  crc 
    2 byte 	| 1 byte	| 2 byte | 100byte| 2 byte 

    độ dài tính theo data + header + crc
*/

/**
 * @brief Create a message object
 * 
 * @param type_mess 
 * @param value 
 * @param data_out 
 * @return uint16_t 
 */
uint16_t create_message(Type_Message type_mess, uint16_t value, uint8_t *data_out)
{
    uint16_t len = 0; // biến đếm xem độ dài của bản tin hiện tại là bao nhiêu byte

    // ---- Start frame ----
    data_out[len++] = 0xAA; // data_out[0]
    data_out[len++] = 0x55; // data_out[1] -> len = 2

    // ---- Type ----
    data_out[len++] = (uint8_t)type_mess; // ép kiểu từ enum sang uint8_t  -> len = 3


    // ---- Length ----
    data_out[len++] = 0x00; // length low byte 0 -> len = 4
    data_out[len++] = 0x00; // length high byte 1 -> len = 5

    if (type_mess == RESPONSE_MESSAGE) // bản tin phản hồi
    {
        uint8_t *val = math.convert.uint16_to_bytes(value); // 2 byte
        data_out[len++] = val[0]; // data out[6]
        data_out[len++] = val[1]; // data out[7]
    }

    // biến đếm hiện tại đang là 7

    // length tổng cả bản tin  = 2 start + 1 type mess + 2 byte length + data + check sum 
    uint16_t length_total = len + 2; // +2 byte checksum -> length tổng của cả bản tin đấy 

    //little endian
    data_out[4] = (uint8_t)(length_total >> 8);
    data_out[3] = (uint8_t)(length_total & 0xFF);

    uint16_t checksum = caculate_checksum(data_out, len);


    uint8_t *chk = math.convert.uint16_to_bytes(checksum);
    data_out[len++] = chk[0];
    data_out[len++] = chk[1];

    return len; // độ dài thật của bản tin 
}

/**
 * @brief Create a message float object
 * 
 * @param type_mess 
 * @param value 
 * @param data_out  array data out of message
 * @return uint16_t 
 */
uint16_t create_message_float (Type_Message type_mess, float value, uint8_t *data_out)
{
    uint16_t len = 0;

    // ---- Start frame ----
    data_out[len++] = 0xAA;
    data_out[len++] = 0x55;

    // ---- Type ----
    data_out[len++] = (uint8_t)type_mess;

    data_out[len++] = 0x00; // length low
    data_out[len++] = 0x00; // length high

    // float 4 byte 
    if (type_mess == RESPONSE_MESSAGE)
    {
        uint8_t *val = math.convert.float_to_bytes(value);
        data_out[len++] = val[0];
        data_out[len++] = val[1];
        data_out[len++] = val[2];
        data_out[len++] = val[3];
    }

    uint16_t length_total = len + 2; // +2 byte checksum
    data_out[4] = (uint8_t)(length_total >> 8);
    data_out[3] = (uint8_t)(length_total & 0xFF);

    uint16_t checksum = caculate_checksum(data_out, len);
    uint8_t *chk = math.convert.uint16_to_bytes(checksum);
    data_out[len++] = chk[0];
    data_out[len++] = chk[1];

    return len; // return length of message
}

    /**
     * @brief checksum 16-bit ~ 2 byte
     */
    uint16_t caculate_checksum(uint8_t *data, uint16_t length_data)
{
    uint32_t sum = 0; // tương đương với int nhưng mà là kiểu dữ liệu không dấu 
    for (uint16_t i = 0; i < length_data; i++)
        sum += data[i]; //tính tổng dữ liệu ở trong mảng data truyền vào
    return (uint16_t)(sum & 0xFFFF);
}
