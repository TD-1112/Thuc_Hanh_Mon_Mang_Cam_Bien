#ifndef __LIB_DHT11__
#define __LIB_DHT11__

#include "stm32f10x.h"
#include "lib_timer2.h"
#include <stdbool.h>

typedef struct
{
    uint8_t temp; // 1byte du lieu
    uint8_t humi; // 1byte du lieu
    float temp_f;
    float humi_f;
} value_dht11;

void DHT11_Init(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin);
bool DHT11_Read(value_dht11 *value);

#endif
