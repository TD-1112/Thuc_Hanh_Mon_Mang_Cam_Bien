#ifndef __FSM__
#define __FSM__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h" // Device header

#include "math.h"
#include "message.h"

#define START_BYTE 0xAA
#define START_BYTE_FOLLOW 0x55
#define FRAME_HEADER_SIZE 5
#define FRAME_MIN_LENGTH 7
#define FSM_MAX_FRAME_SIZE 200

#define TRUE 1
#define FALSE 0
#define COUNTER_TIMEOUT 2000

	typedef enum
	{
		FSM_STATE_START = 0,
		FSM_STATE_WAIT = 1,
		FSM_STATE_END = 2,
	} fsmListState_e;

	typedef enum
	{
		FSM_STATE_CHANGE_VALUE_START_FRAME = 0,
		FSM_STATE_CHANGE_VALUE_WAIT = 2,
		FSM_STATE_CHANGE_VALUE_END = FRAME_HEADER_SIZE,
	} fsmValueNextStep_e;

	extern int32_t timeout_wait;
	extern int16_t length_message;

	uint16_t is_message(uint16_t *lenght);
	void fsm_get_message(uint8_t datain, uint8_t arr_message[]);

#ifdef __cplusplus
}
#endif

#endif
