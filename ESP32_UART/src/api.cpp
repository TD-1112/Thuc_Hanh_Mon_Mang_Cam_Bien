#include "api.h"

// Hàm decode
bool decode_message(uint8_t *message, uint16_t length, Type_Message *type_message, uint16_t *data_value)
{
    if (length < FRAME_MIN_LENGTH)
        return false;
    if (message[0] != 0xAA || message[1] != 0x55)
        return false;

    *type_message = (Type_Message)message[2];
    uint16_t msg_length = message[3] << 8 | message[4];
    if (msg_length != length)
        return false;

    uint16_t checksum_received = message[length - 2] | (message[length - 1] << 8);
    uint16_t checksum_calc = caculate_checksum(message, length - 2);
    if (checksum_calc != checksum_received)
        return false;

    if (*type_message == RESPONSE_MESSAGE && length >= 9)
    {
        uint16_t value_le = message[5] | (message[6] << 8);
        *data_value = value_le;
        return true;
    }

    return false;
}

