#include "app_i2c.h"

/**
 * Hàm cấu hình giao tiếp I2C1
 * Thiết lập chân PB6 (SCL) và PB7 (SDA) để giao tiếp với cảm biến
 */
void I2C1_config (void)
{
    // Kích hoạt đồng hồ cho GPIOB và AFIO (chức năng luân phiên) trên bus APB2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    // Kích hoạt đồng hồ cho I2C1 trên bus APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // Khai báo biến cấu trúc để cấu hình GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    // Khai báo biến cấu trúc để cấu hình I2C
    I2C_InitTypeDef I2C_InitStructure;
    
    // Cấu hình chân GPIO cho I2C
    // Chế độ Alternate Function Open Drain (bắt buộc cho I2C)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    // Sử dụng chân PB6 (SCL) và PB7 (SDA)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    // Tốc độ truyền dữ liệu tối đa 50MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // Áp dụng cấu hình cho GPIOB
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Cấu hình module I2C
    // Tắt chế độ tự động xác nhận (ACK)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
    // Sử dụng địa chỉ 7-bit (chuẩn I2C thông thường)
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    // Thiết lập tốc độ clock 200kHz (giữa standard mode 100kHz và fast mode 400kHz)
    I2C_InitStructure.I2C_ClockSpeed = 200000;
    // Chu kỳ xung tỉ lệ 1:2 (tỉ lệ thời gian cao:thấp của xung clock)
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    // Hoạt động ở chế độ I2C tiêu chuẩn
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    // Địa chỉ của vi điều khiển khi hoạt động ở chế độ slave
    I2C_InitStructure.I2C_OwnAddress1 = 0x0B;
    
    // Áp dụng cấu hình cho I2C1
    I2C_Init(I2C1,&I2C_InitStructure);
    // Kích hoạt I2C1
    I2C_Cmd(I2C1,ENABLE);
}

/**
 * Hàm ghi dữ liệu tới thiết bị I2C
 * @param HW_address: Địa chỉ thiết bị (7 bit)
 * @param sub: Địa chỉ thanh ghi con (0xFF nếu không sử dụng)
 * @param data: Dữ liệu cần ghi
 */
void I2C1_write(unsigned char HW_address, unsigned char sub, unsigned char data)
{
    // Biến đếm timeout để tránh treo hệ thống
    int ticks = I2C_TIMEOUT;
    
    // Tạo điều kiện START
    I2C_GenerateSTART(I2C1, ENABLE);
    // Đợi cho đến khi trạng thái MASTER được chọn
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)&&ticks) {ticks--;}
    // Thoát nếu quá thời gian chờ
    if (ticks == 0) return;
    ticks = I2C_TIMEOUT;
    
    // Gửi địa chỉ thiết bị ở chế độ truyền (TRANSMITTER)
    I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
    // Đợi cho đến khi chế độ MASTER TRANSMITTER được chọn
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && ticks) 
	{
		ticks--;
	}
    if (ticks == 0) return;
    ticks = I2C_TIMEOUT;
    
    // Nếu sub != 0xFF thì gửi địa chỉ thanh ghi con
    if (sub != 0xFF)
    {
        // Gửi địa chỉ thanh ghi con
        I2C_SendData(I2C1, sub);
        // Đợi cho đến khi byte được truyền đi
        while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&ticks) {ticks--;}
        if (ticks == 0) return;
        ticks = I2C_TIMEOUT;
    }
    
    // Gửi dữ liệu cần ghi
    I2C_SendData(I2C1, data);
    // Đợi cho đến khi byte được truyền đi
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&ticks) {ticks--;}
    if (ticks == 0) return;
    ticks = I2C_TIMEOUT;
    
    // Tạo điều kiện STOP để kết thúc giao tiếp
    I2C_GenerateSTOP(I2C1, ENABLE);
    // Đợi cho đến khi bus I2C không còn bận
    while((I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) && ticks) {ticks--;}
    if (ticks == 0) return;
}

/**
 * Hàm đọc 1 byte từ thiết bị I2C
 * @param HW_address: Địa chỉ thiết bị (7 bit)
 * @param sub: Địa chỉ thanh ghi con (0xFF nếu không sử dụng)
 * @return: Giá trị đọc được hoặc 0 nếu xảy ra lỗi
 */
char I2C1_read (unsigned char HW_address, unsigned char sub)
{
    char data;    
    // Tạo điều kiện START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    // Gửi địa chỉ thiết bị ở chế độ truyền để chọn thanh ghi
    I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
    // Nếu sub != 0xFF thì gửi địa chỉ thanh ghi con
    if (sub != 0xFF)
    {   
        I2C_SendData(I2C1, sub);
        while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    }
    // Tạo điều kiện START lặp lại (repeated START) để chuyển sang chế độ nhận
    I2C_GenerateSTART(I2C1, ENABLE);
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
    // Gửi địa chỉ thiết bị ở chế độ nhận (RECEIVER)
    I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Receiver);
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
    // Đợi cho đến khi nhận được byte dữ liệu
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)));
    // Đọc dữ liệu từ thanh ghi dữ liệu
    data = I2C1->DR;
    // Cấu hình vị trí NACK (không xác nhận)
    I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
    // Tạo điều kiện STOP để kết thúc giao tiếp
    I2C_GenerateSTOP(I2C1, ENABLE);
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    // Trả về dữ liệu đã đọc
    return data;
}

/**
 * Hàm đọc nhiều byte từ thiết bị I2C vào buffer
 * @param HW_address: Địa chỉ thiết bị (7 bit)
 * @param sub: Địa chỉ thanh ghi con (0xFF nếu không sử dụng)
 * @param p_buf: Con trỏ đến buffer lưu dữ liệu
 * @param buf_size: Kích thước buffer (số byte cần đọc)
 */
void I2C1_read_buf (unsigned char HW_address, unsigned char sub, unsigned char * p_buf, unsigned char buf_size)
{
    // Tạo điều kiện START
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    // Gửi địa chỉ thiết bị ở chế độ truyền để chọn thanh ghi
    I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // Nếu sub != 0xFF thì gửi địa chỉ thanh ghi con
    if (sub != 0xFF)
    {   
        I2C_SendData(I2C1, sub);
        while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
    }
    // Tạo điều kiện START lặp lại (repeated START) để chuyển sang chế độ nhận
    I2C_GenerateSTART(I2C1, ENABLE);
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
    // Bật chế độ ACK để xác nhận dữ liệu nhận được
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    // Gửi địa chỉ thiết bị ở chế độ nhận (RECEIVER)
    I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Receiver);
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
    // Đọc nhiều byte dữ liệu vvào buffer
    for (uint8_t i=0; i<buf_size; i++)
    {
        // Đợi cho đến khi nhận được byte dữ liệu
        while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)));
        // Lưu dữ liệu vào buffer
        p_buf[i] = I2C1->DR;
    }
    // Tắt chế độ ACK sau khi đọc xong tất cả các byte
    I2C_AcknowledgeConfig(I2C1,DISABLE);
    // Cấu hình vị trí NACK
    I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
    // Tạo điều kiện STOP để kết thúc giao tiếp
    I2C_GenerateSTOP(I2C1, ENABLE);
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

}

