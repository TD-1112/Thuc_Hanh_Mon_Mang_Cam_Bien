#ifndef __API__
#define __API__

#include <Arduino.h>
#include "fsm.h"
#include "message.h"

bool decode_message(uint8_t *message, uint16_t length, Type_Message *type_message, uint16_t *data_value);

#endif