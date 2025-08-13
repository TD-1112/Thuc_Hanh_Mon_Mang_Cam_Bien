#ifndef __LIB_MESSAGE_H
#define __LIB_MESSAGE_H

#include "lib_convert.h"

// Frame of your msg
typedef struct {
    uint16_t StartFrame;        
    uint16_t TypeMessage;       
    uint16_t Length;
    uint8_t Data[12];
    uint16_t CheckFrame;
} frame_Message_t;

#define START_BYTES                0xAA55       /* Start your msg*/
#define LENGTH_DEFAULT             0X0004       /* Total bytes of Length + Data[port] + CheckFrame*/
#define LENGTH_MESSAGE_RESPOND_LUX 0x000A       /* Total bytes of Length + Data[port] + Data[TypeSensor] + (4bytes) Data[LUX] + CheckFrame*/
#define ERROR                      0x0000       /* Variable of error*/
#define LENGTH_DEFAULT_CHECK_SUM   0x0002       /* Total bytes of CheckFram*/ 

typedef enum
{
   TYPE_MESSAGE_SET_PORT             = 0x01,
   TYPE_MESSAGE_CONFIRM_PORT         = 0x02,
   TYPE_MESSAGE_ASK_DATA             = 0x03,
   TYPE_MESSAGE_RESPOND_DATA         = 0x04, 
} type_Message_e;

typedef enum
{
   PORT_A = 0x01,
   PORT_B = 0x02,
} port_e;

typedef enum
{
   SENSOR_LUX      = 0x01,
   SENSOR_RTC      = 0x02,
   SENSOR_NTC      = 0x03,
   SENSOR_RES      = 0x04,
} type_Sensor_e;

uint8_t Message_Create_Frame_Respond_LUX(uint8_t datain_port,float datain_value, uint8_t *dataout);
float Get_Data_Sensor_LUX(frame_Message_t datain);
uint8_t Detect_Frame(uint8_t *datain, frame_Message_t *dataout);
uint16_t Check_Sum(uint8_t *buf, uint8_t len);

#endif