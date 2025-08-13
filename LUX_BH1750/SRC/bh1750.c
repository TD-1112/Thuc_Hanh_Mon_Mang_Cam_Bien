#include "bh1750.h"


void BH1750_Init (void)
{
    // Khởi tạo và cấu hình giao tiếp I2C
    I2C1_config();
    // Gửi lệnh bật nguồn đến cảm biến BH1750 theo địa chỉ BH1750_ADDR
    I2C1_write(BH1750_ADDR, 0xFF, BH1750_POWER_ON);
    
}

float BH1750_ReadLux (void)
{
    uint8_t tmp8[2];  // Mảng lưu 2 byte dữ liệu nhận được
    float ret;         // Biến lưu giá trị lux kết quả
    
    // Đọc dữ liệu từ cảm biến ở chế độ đo một lần với độ phân giải cao
    I2C1_read_buf(BH1750_ADDR, BH1750_ONE_H_MODE, tmp8, 2);
    
    // Chuyển đổi 2 byte thành giá trị 16-bit và chia cho 1.2 để có giá trị lux
    // byte đầu tiên là byte cao (MSB), byte thứ hai là byte thấp (LSB)
    ret = (float)(tmp8[0]<<8 | tmp8[1])/1.2;
     
    return ret;  // Trả về giá trị cường độ ánh sáng đơn vị lux
}


