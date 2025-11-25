#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "stm32f10x.h"
#include <stdint.h>

// Start Frame
#define START_FRAME_HIGH 0xAA
#define START_FRAME_LOW 0x55

// Message Types
typedef enum
{
    MSG_TYPE_CMD = 0x00,
    MSG_TYPE_DATA = 0x01
} MessageType;

// Action Codes for CMD Message
typedef enum
{
    ACTION_IN = 0x00,
    ACTION_OUT = 0x01,
    ACTION_CHECK = 0x03,
    ACTION_DONE = 0xFF
} ActionCode;

// Tags for DATA Message
typedef enum
{
    TAG_INFOR = 0x00,
    TAG_LOCATION = 0x01
} TagType;

// Product Codes
typedef enum
{
    PRODUCT_SP1 = 0x01,
    PRODUCT_SP2 = 0x02,
    PRODUCT_SP3 = 0x03
} ProductCode;

// Status Codes
typedef enum
{
    STATUS_VALID = 0x00,
    STATUS_INVALID = 0x01
} StatusCode;

// Data Structures
typedef struct
{
    uint8_t uid[4];
    uint8_t product_code;
    uint8_t status;
} Data_Infor;

typedef struct
{
    uint8_t region;
    uint8_t row;
    uint8_t shelf;
} Data_Location;

typedef struct
{
    MessageType type;
    ActionCode action; // Valid if type == CMD
    TagType tag;       // Valid if type == DATA
    Data_Infor info;   // Valid if type == DATA && tag == INFOR
    Data_Location loc; // Valid if type == DATA && tag == LOCATION
    uint8_t is_valid;  // 1 if checksum valid, 0 otherwise
} DecodedMessage;

// Function Prototypes
uint16_t create_cmd_message(ActionCode action, uint8_t *buffer);
uint16_t create_data_infor_message(Data_Infor *info, uint8_t *buffer);
uint16_t create_data_location_message(Data_Location *loc, uint8_t *buffer);
uint16_t calculate_checksum(uint8_t *data, uint16_t length_data);
DecodedMessage message_decode(uint8_t *buffer, uint16_t length);

#endif
