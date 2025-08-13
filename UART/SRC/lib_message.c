#include "lib_message.h"

/*
    @brief:               Create Frame msg to respond data of lux.
    @node:                To create a function that generates an array containing the 
                          information for a request/response message related to sensor data 
                          (such as message type, sensor type, etc.)
    @param datain_value:  data of sensor lux.
    @param dataout:       array.
    @return:              length of dataout.
*/
uint8_t Message_Create_Frame_Respond_LUX(uint8_t datain_port, float datain_value, uint8_t *dataout)
{
  uint8_t count_arr = 0;
  
  // Write Start Frame (0xAA55) in Big-Endian
  dataout[count_arr++] = 0xAA;  // High byte
  dataout[count_arr++] = 0x55;  // Low byte
  
  // Write Type Message in Big-Endian
  dataout[count_arr++] = (TYPE_MESSAGE_RESPOND_DATA >> 8) & 0xFF;  // High byte
  dataout[count_arr++] = TYPE_MESSAGE_RESPOND_DATA & 0xFF;         // Low byte
  
  // Write Length in Big-Endian
  dataout[count_arr++] = (LENGTH_MESSAGE_RESPOND_LUX >> 8) & 0xFF; // High byte
  dataout[count_arr++] = LENGTH_MESSAGE_RESPOND_LUX & 0xFF;        // Low byte
  
  // Write port and sensor type
  dataout[count_arr++] = datain_port;
  dataout[count_arr++] = SENSOR_LUX;
  
  // Convert float and write in Big-Endian
  uint8_t *data_convert = Convert_Float_To_Bytes(datain_value);
  // Reverse byte order for Big-Endian
  for (int i = 3; i >= 0; i--)
  {
    dataout[count_arr++] = data_convert[i];
  }
  
  // Calculate checksum
  uint16_t checksum = Check_Sum(dataout, count_arr);
  
  // Write checksum in Big-Endian
  dataout[count_arr++] = (checksum >> 8) & 0xFF;  // High byte
  dataout[count_arr++] = checksum & 0xFF;         // Low byte
  
  return count_arr;
}

float Get_Data_Sensor_LUX(frame_Message_t datain)
{
  return Convert_Bytes_To_Float(datain.Data[2], datain.Data[3], datain.Data[4], datain.Data[5]);
}

uint8_t Detect_Frame(uint8_t *datain, frame_Message_t *dataout)
{
    uint8_t count_data_in = 0;

    // Read Start Frame in Big-Endian
    dataout->StartFrame = ((uint16_t)datain[count_data_in] << 8) | datain[count_data_in + 1];
    count_data_in += 2;
    if (dataout->StartFrame != START_BYTES)
    {
        return 0;
    }

    // Read Type Message in Big-Endian
    dataout->TypeMessage = ((uint16_t)datain[count_data_in] << 8) | datain[count_data_in + 1];
    count_data_in += 2;

    // Read Length in Big-Endian
    dataout->Length = ((uint16_t)datain[count_data_in] << 8) | datain[count_data_in + 1];
    count_data_in += 2;

    // Copy payload data
    for (uint8_t i = 0; i < dataout->Length - 4; i++)
    {
        dataout->Data[i] = datain[count_data_in++];
    }

    // Read Checksum in Big-Endian
    dataout->CheckFrame = ((uint16_t)datain[count_data_in] << 8) | datain[count_data_in + 1];

    return dataout->TypeMessage;
}

uint16_t Check_Sum(uint8_t *buf, uint8_t len)
{
    uint16_t crc = 0xFFFF, pos = 0, i = 0;
    for (pos = 0; pos < len; pos++)
    {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (i = 8; i != 0; i--)   // Loop over each bit
        {
            if ((crc & 0x0001) != 0) // If the LSB is set
            {
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else // Else LSB is not set
            {
                crc >>= 1; // Just shift right
            }
        }
    }
    return crc;
}
