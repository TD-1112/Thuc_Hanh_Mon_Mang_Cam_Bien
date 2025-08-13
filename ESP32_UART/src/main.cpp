#include <Arduino.h>
#include "lib_convert.h"

#define BUFFER_SIZE 20
#define HEADER_1 0xAA
#define HEADER_2 0x55
#define MESSAGE_LENGTH 14  // Độ dài bản tin cần nhận

void processMessage(uint8_t* data);
void loop_lux_sensor(void);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.println("Start UART Monitor");
}



void loop() 
{
  loop_lux_sensor();
  //delay(1000);
}

void processMessage(uint8_t* data) {
  // AA 55 00 04 00 0A 01 01 41 B0 66 66 96 F5
  //                         ^ Bắt đầu từ đây
  // Đảo thứ tự byte từ Little Endian sang Big Endian
  float value = Convert_Bytes_To_Float(data[11], data[10], data[9], data[8]);
  
  Serial.print("Raw bytes: ");
  for(int i = 8; i < 12; i++) {
      if(data[i] < 0x10) Serial.print("0");
      Serial.print(data[i], HEX);
      Serial.print(" ");
  }
  Serial.println();
  
  Serial.print("Converted float value: ");
  Serial.println(value, 2); // In ra với 2 chữ số thập phân
}

void loop_lux_sensor(void)
{

  static uint8_t data[BUFFER_SIZE];
  static uint8_t length_data_receive = 0;
  
  while (Serial2.available()) {
    uint8_t incomingByte = Serial2.read();
    
    // Debug mỗi byte nhận được
   // Serial.print("Byte received: 0x");
    //if(incomingByte < 0x10) Serial.print("0");
   // Serial.println(incomingByte, HEX);
    
    data[length_data_receive] = incomingByte;
    length_data_receive++;
    
    // Kiểm tra header và xử lý bản tin hoàn chỉnh
    if (length_data_receive >= 2) {
      if (data[0] == HEADER_1 && data[1] == HEADER_2) {
        if (length_data_receive == MESSAGE_LENGTH) {
          Serial.println("\nComplete message received!");
          Serial.print("Message: ");
          for (int i = 0; i < MESSAGE_LENGTH; i++) {
            if(data[i] < 0x10) Serial.print("0");
            Serial.print(data[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
          
          // Xử lý 4 byte thành float
          processMessage(data);
          
          // Reset buffer sau khi nhận đủ bản tin
          length_data_receive = 0;
        }
      } else if (data[length_data_receive-1] == HEADER_1) {
        // Nếu tìm thấy header mới, dịch buffer
        data[0] = HEADER_1;
        length_data_receive = 1;
      }
    }
    
    // Kiểm tra overflow buffer
    if (length_data_receive >= BUFFER_SIZE) {
      Serial.println("Buffer overflow - resetting");
      length_data_receive = 0;
    }
  }
}