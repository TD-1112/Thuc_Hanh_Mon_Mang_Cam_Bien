#include "dht11.h"
#include "uart.h"

// Hàm đọc một byte từ DHT11
static uint8_t DHT11_ReadByte(void)
{
    uint8_t byte = 0;
    uint16_t u16Tim;
    
    for (int i = 0; i < 8; ++i) {
        // Chờ PB12 lên cao
        TIM_SetCounter(TIM2, 0);
        while (TIM_GetCounter(TIM2) < 65) {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
                break;
        }
        u16Tim = TIM_GetCounter(TIM2);
        
        // Chờ PB12 xuống thấp
        TIM_SetCounter(TIM2, 0);
        while (TIM_GetCounter(TIM2) < 80) {
            if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
                break;
        }
        u16Tim = TIM_GetCounter(TIM2);
        
        // Xử lý bit
        byte <<= 1;
        if (u16Tim > 45) {
            byte |= 1;  // Bit 1
        }
    }
    
    return byte;
}

void DHT11_Init(void)
{
    GPIO_InitTypeDef gpioInit;
    // Enable clock for GPIOC and GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    // LED DEBUG PC13 - OUTPUT
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);
    // DHT11 PB12 - OUTPUT
    gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Pin = GPIO_Pin_12;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
}

void DHT11_Read(void)
{
    uint16_t u16Tim;
    uint8_t  u8Buff[5];
    uint8_t  u8CheckSum;
    
    // Gửi tín hiệu bắt đầu
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    Delay_Ms(20);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    
    // Chờ phản hồi từ DHT11
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 10) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
            break;
        }
    }
    
    // Chờ PB12 low
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 45) {
        if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
            break;
        }
    }
    
    // Chờ PB12 high
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 90) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
            break;
        }
    }
    
    // Chờ PB12 low
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 95) {
        if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
            break;
        }
    }
    
    // Đọc 5 byte dữ liệu
    for (int i = 0; i < 5; i++) {
        u8Buff[i] = DHT11_ReadByte();
    }
    
    // Kiểm tra checksum nhưng không dừng khi có lỗi
    u8CheckSum = u8Buff[0] + u8Buff[1] + u8Buff[2] + u8Buff[3];
    
    // Debug qua UART bất kể checksum đúng hay sai
    USART1_Send_String("Temperature: ");
    USART1_Send_Number(u8Buff[2]);
    USART1_Send_String("*C\n");
    USART1_Send_String("Humidity: ");
    USART1_Send_Number(u8Buff[0]);
    USART1_Send_String("%\n");
    
    // Thông báo nếu có lỗi checksum
    if (u8CheckSum != u8Buff[4]) {
        USART1_Send_String("Checksum ERROR!\n");
    }
}
