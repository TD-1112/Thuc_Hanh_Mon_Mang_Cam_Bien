#include "lib_dht11.h"
#include "lib_math.h"

static GPIO_TypeDef *DHT11_Port;
static uint16_t DHT11_Pin;

// macro
#define DHT11_OUT_LOW() (DHT11_Port->BRR = DHT11_Pin)
#define DHT11_OUT_HIGH() (DHT11_Port->BSRR = DHT11_Pin)

#define DHT11_READ() GPIO_ReadInputDataBit(DHT11_Port, DHT11_Pin)

static void DHT11_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_Port, &GPIO_InitStructure);
}

static void DHT11_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_Port, &GPIO_InitStructure);
}

static void DHT11_Rst(void)
{
    DHT11_IO_OUT();
    DHT11_OUT_LOW();
    Delay_ms(20);
    DHT11_OUT_HIGH();
    Delay_us(30);
}

static bool DHT11_Check(void)
{
    uint8_t time = 0;
    DHT11_IO_IN();

    while (DHT11_READ() && time < 100)
    {
        time++;
        Delay_us(1);
    }
    if (time >= 100)
        return false;

    time = 0;

    while (!DHT11_READ() && time < 100)
    {
        time++;
        Delay_us(1);
    }
    if (time >= 100)
        return false;

    return true;
}

/**
 * ham tra ve xem bit la 1 hay la 0
 */
static uint8_t DHT11_ReadBit(void)
{
    uint8_t time = 0;

    while (DHT11_READ() && time < 100)
    {
        time++;
        Delay_us(1);
    }

    time = 0;
    while (!DHT11_READ() && time < 100)
    {
        time++;
        Delay_us(1);
    }

    Delay_us(40);
    return DHT11_READ() ? 1 : 0; // bit la 1 hay la 0 va khoang thoi gian de coi no la bit 1 hay la 0 la bao nhieu us
}

// 1 byte =  8 bit -> uint8_t(chiem 1 byte)
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, data = 0;
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= DHT11_ReadBit();
    }
    return data; // -> tra ve 1 byte du lieu
}

void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    DHT11_Port = GPIOx;
    DHT11_Pin = GPIO_Pin;

    if (GPIOx == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (GPIOx == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (GPIOx == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    // mcu signal start -> dht11
    DHT11_IO_OUT();
    DHT11_OUT_HIGH();
}

// bool -> true false
bool DHT11_Read(value_dht11 *value)
{
    uint8_t buf[5]; // 0 -> 4

    DHT11_Rst();
    if (!DHT11_Check())
        return false;

    // 1 frame data co 5 byte
    for (int i = 0; i < 5; i++)
    {
        buf[i] = DHT11_ReadByte();
    }

    // check sum no la de check xem cai data co bi loi hay ko
    if (buf[4] != (uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]))
        return false;

    value->humi = buf[0]; // 1 byte do am
    value->temp = buf[2]; // 1 byte nhiet do

    value->humi_f = buf[0] + buf[1] * 0.1f;
    value->temp_f = buf[2] + buf[3] * 0.1f;

    return true;
}
