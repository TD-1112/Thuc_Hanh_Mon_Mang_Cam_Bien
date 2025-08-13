#include "lib_convert.h"


/*
    @brief: Convert variable from int to bytes.
    @param data: A int variable was needed converting to bytes.
    @return: bytes variable.
*/

// uint8_t *Convert_Int_To_Bytes(int data)
// {
//     static data_convert_int_and_bytes input;
//     input.data_int = data;
//     return input.data_bytes;
// }

/*
    @brief: Convert variable from float to bytes.
    @param data: A float variable was needed converting to bytes.
    @return: bytes variable.
*/
uint8_t *Convert_Float_To_Bytes(float data)
{
    static data_convert_float_and_bytes input;
    input.data_float = data;
    return input.data_bytes;
}
/*
    @brief: Convert variable from uint16 to bytes.
    @param data: A uint16 variable was needed converting to bytes.
    @return: bytes variable.
*/

uint8_t *Convert_Uint16_To_Bytes(uint16_t data)
{
    static data_convert_uint16_and_bytes input;
    input.data_uint16 = data;
    return input.data_bytes;
}
/*
    @brief: Convert variables from bytes to int.
    @param data1: First variable of bytes.
    @param data2: Second variable of bytes.
    @param data3: Third variable of bytes.
    @param data4: Fourth variable of bytes.
    @return: Int variable.
*/

// int Convert_Bytes_To_Int(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
// {
//     data_convert_int_and_bytes input;
//     input.data_bytes[0] = data1;
//     input.data_bytes[1] = data2;
//     input.data_bytes[2] = data3;
//     input.data_bytes[3] = data4;
//     return input.data_int;
// }

/*
    @brief: Convert variables from bytes to float.
    @param data1: First variable of bytes.
    @param data2: Second variable of bytes.
    @param data3: Third variable of bytes.
    @param data4: Fourth variable of bytes.
    @return: float variable.
*/

float Convert_Bytes_To_Float(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    data_convert_float_and_bytes input;
    input.data_bytes[0] = data1;
    input.data_bytes[1] = data2;
    input.data_bytes[2] = data3;
    input.data_bytes[3] = data4;
    return input.data_float;
}

/*
    @brief: Convert variables from bytes to uint16.
    @param data1: First variable of bytes.
    @param data2: Second variable of bytes.
    @return: uint16 variable.
*/

 uint16_t Convert_Bytes_To_Uint16(uint8_t data1, uint8_t data2)
 {
     data_convert_uint16_and_bytes input;
     input.data_bytes[0] = data1;
     input.data_bytes[1] = data2;
     return input.data_uint16;
 }
