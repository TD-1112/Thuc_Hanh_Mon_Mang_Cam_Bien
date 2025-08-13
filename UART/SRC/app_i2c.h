#ifndef _APP_I2C_H
#define _APP_I2C_H

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "bh1750.h"

void I2C1_config (void);
void I2C1_write(unsigned char HW_address, unsigned char sub, unsigned char data);
char I2C1_read (unsigned char HW_address, unsigned char sub);
void I2C1_read_buf (unsigned char HW_address, unsigned char sub, unsigned char * p_buf, unsigned char buf_size);


#endif
