#ifndef __DHT11__
#define __DHT11__

#include "stm32f10x.h"                  // Device header
#include "delays.h"

void DHT11_Init(void);
void DHT11_Read(void);

#endif