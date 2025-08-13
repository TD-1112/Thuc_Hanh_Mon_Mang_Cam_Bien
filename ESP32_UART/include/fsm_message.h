#ifndef FSM_MESSAGE_H
#define FSM_MESSAGE_H

#include <stdio.h>
#include <lib_convert.h>
#include <lib_message.h>

#define TRUE  1
#define FALSE 0

// Định nghĩa các trạng thái của FSM
typedef enum
{
    FSM_STATE_START_FRAME = 0,  // Trạng thái đọc header
    FSM_STATE_TYPE_MESS  = 1,   // Trạng thái đọc loại message
    FSM_STATE_LENGTH     = 2,   // Trạng thái đọc độ dài
    FSM_STATE_DATA       = 3,   // Trạng thái đọc dữ liệu
    FSM_STATE_END        = 4,   // Trạng thái kết thúc và checksum
} fsm_List_State_e;

// Định nghĩa các vị trí byte trong frame
typedef enum
{
    FSM_STATE_CHANGE_VALUE_START_FRAME = 0,   // Vị trí bắt đầu frame
    FSM_STATE_CHANGE_VALUE_TYPE_MESS   = 2,   // Vị trí type message
    FSM_STATE_CHANGE_VALUE_LENGTH      = 4,   // Vị trí length
    FSM_STATE_CHANGE_VALUE_DATA        = 6,   // Vị trí bắt đầu data
    FSM_STATE_CHANGE_VALUE_END_DATA_LUX = (2 + LENGTH_MESSAGE_RESPOND_LUX), // Vị trí kết thúc data LUX
} fsm_Value_Next_Step_e;

// Khai báo các hàm
void Fsm_Get_Message(uint8_t datain, uint8_t dataout[]);
void Clear_All_State_Fsm(void);
uint8_t Check_Fsm_Flag_New_Message(void);

#endif // FSM_MESSAGE_H
