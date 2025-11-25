#include "message.h"
#include "math.h"

// Note: STM32 is Little Endian.
// Multi-byte fields (Length, Checksum) are sent LSB first using math.convert;

/**
 * @brief checksum 16-bit
 */
uint16_t calculate_checksum(uint8_t *data, uint16_t length_data)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < length_data; i++)
        sum += data[i];

    uint8_t lsb = (uint8_t)(sum & 0xFF);
    uint8_t checksum = (uint8_t)((~lsb) + 1);

    return (uint16_t)checksum;
}

/**
 * @brief Create Command Message
 * Structure: | Start(2) | Type(1) | Action(1) | Length(2) | Checksum(2) |
 */
uint16_t create_cmd_message(ActionCode action, uint8_t *buffer)
{
    uint16_t idx = 0;

    // Start Frame
    buffer[idx++] = START_FRAME_HIGH;
    buffer[idx++] = START_FRAME_LOW;

    // Type Message
    buffer[idx++] = MSG_TYPE_CMD;

    // Action Code
    buffer[idx++] = (uint8_t)action;

    // Length: tổng độ dài bản tin (header + length + checksum)
    // CMD: Start(2) + Type(1) + Action(1) + Length(2) + Checksum(2) = 8 bytes
    uint16_t total_length = 8;
    uint8_t *len_bytes = math.convert.uint16_to_bytes(total_length);
    buffer[idx++] = len_bytes[0];
    buffer[idx++] = len_bytes[1];

    // Checksum
    uint16_t checksum = calculate_checksum(buffer, idx);
    uint8_t *chk_bytes = math.convert.uint16_to_bytes(checksum);
    buffer[idx++] = chk_bytes[0];
    buffer[idx++] = chk_bytes[1];

    return idx; // Total length
}

/**
 * @brief Create Data Message (Info)
 * Structure: | Start(2) | Type(1) | Tag(1) | Length(2) | Data(6) | Checksum(2) |
 */
uint16_t create_data_infor_message(Data_Infor *info, uint8_t *buffer)
{
    uint16_t idx = 0;

    // Start Frame
    buffer[idx++] = START_FRAME_HIGH;
    buffer[idx++] = START_FRAME_LOW;

    // Type Message
    buffer[idx++] = MSG_TYPE_DATA;

    // Tag
    buffer[idx++] = TAG_INFOR;

    // Length: tổng độ dài bản tin (header + data + checksum)
    // DATA INFOR: Start(2) + Type(1) + Tag(1) + Length(2) + Data(6) + Checksum(2) = 13 bytes
    uint16_t total_length = 13;
    uint8_t *len_bytes = math.convert.uint16_to_bytes(total_length);
    buffer[idx++] = len_bytes[0];
    buffer[idx++] = len_bytes[1];

    // Data Payload
    for (int i = 0; i < 4; i++)
    {
        buffer[idx++] = info->uid[i];
    }
    buffer[idx++] = info->product_code;
    buffer[idx++] = info->status;

    // Checksum
    uint16_t checksum = calculate_checksum(buffer, idx);
    uint8_t *chk_bytes = math.convert.uint16_to_bytes(checksum);
    buffer[idx++] = chk_bytes[0];
    buffer[idx++] = chk_bytes[1];

    return idx;
}

/**
 * @brief Create Data Message (Location)
 * Structure: | Start(2) | Type(1) | Tag(1) | Length(2) | Data(3) | Checksum(2) |
 */
uint16_t create_data_location_message(Data_Location *loc, uint8_t *buffer)
{
    uint16_t idx = 0;

    // Start Frame
    buffer[idx++] = START_FRAME_HIGH;
    buffer[idx++] = START_FRAME_LOW;

    // Type Message
    buffer[idx++] = MSG_TYPE_DATA;

    // Tag
    buffer[idx++] = TAG_LOCATION;

    // Length: tổng độ dài bản tin (header + data + checksum)
    // DATA LOCATION: Start(2) + Type(1) + Tag(1) + Length(2) + Data(3) + Checksum(2) = 11 bytes
    uint16_t total_length = 11;
    uint8_t *len_bytes = math.convert.uint16_to_bytes(total_length);
    buffer[idx++] = len_bytes[0];
    buffer[idx++] = len_bytes[1];

    // Data Payload
    buffer[idx++] = loc->region;
    buffer[idx++] = loc->row;
    buffer[idx++] = loc->shelf;

    // Checksum
    uint16_t checksum = calculate_checksum(buffer, idx);
    uint8_t *chk_bytes = math.convert.uint16_to_bytes(checksum);
    buffer[idx++] = chk_bytes[0];
    buffer[idx++] = chk_bytes[1];

    return idx;
}

/**
 * @brief Decode received message
 */
DecodedMessage message_decode(uint8_t *buffer, uint16_t length)
{
    DecodedMessage msg;
    msg.is_valid = 0;

    // Basic length check (Header 6 bytes + Checksum 2 bytes = 8 bytes min for Data, 8 for Cmd)
    // Actually CMD: Start(2)+Type(1)+Action(1)+Len(2)+Chk(2) = 8 bytes.
    // DATA: Start(2)+Type(1)+Tag(1)+Len(2)+Data(N)+Chk(2) = 8 + N bytes.
    if (length < 8)
        return msg;

    // Check Start Frame
    if (buffer[0] != START_FRAME_HIGH || buffer[1] != START_FRAME_LOW)
        return msg;

    // Verify Checksum
    // Checksum is at the end (last 2 bytes)
    // Calculated checksum is over (length - 2) bytes
    uint16_t received_checksum = (uint16_t)(buffer[length - 2] << 8) | buffer[length - 1];
    // Note: In create functions, we stored checksum as High then Low.
    // buffer[idx++] = (uint8_t)(checksum >> 8);
    // buffer[idx++] = (uint8_t)(checksum & 0xFF);
    // So buffer[len-2] is High, buffer[len-1] is Low.
    // So (High << 8) | Low is correct.

    uint16_t calc_checksum = calculate_checksum(buffer, length - 2);
    if (received_checksum != calc_checksum)
        return msg;

    msg.is_valid = 1;
    msg.type = (MessageType)buffer[2];

    if (msg.type == MSG_TYPE_CMD)
    {
        msg.action = (ActionCode)buffer[3];
    }
    else if (msg.type == MSG_TYPE_DATA)
    {
        msg.tag = (TagType)buffer[3];
        // Length is at buffer[4] (Low) and buffer[5] (High) based on create functions
        // uint8_t *len_bytes = math.convert.uint16_to_bytes(len);
        // buffer[idx++] = len_bytes[0]; // Low
        // buffer[idx++] = len_bytes[1]; // High
        // So buffer[4] is Low, buffer[5] is High.
        // uint16_t data_len = (uint16_t)buffer[4] | ((uint16_t)buffer[5] << 8);

        // Data starts at buffer[6]
        if (msg.tag == TAG_INFOR)
        {
            // UID (4 bytes)
            for (int i = 0; i < 4; i++)
                msg.info.uid[i] = buffer[6 + i];
            msg.info.product_code = buffer[10];
            msg.info.status = buffer[11];
        }
        else if (msg.tag == TAG_LOCATION)
        {
            msg.loc.region = buffer[6];
            msg.loc.row = buffer[7];
            msg.loc.shelf = buffer[8];
        }
    }

    return msg;
}
