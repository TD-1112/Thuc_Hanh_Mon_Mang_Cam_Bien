#include "rfid.h"

/**
 * @brief function to initialize pin cs for RFID (pin 12 on port B)
 *
 */
void pin_cs_init(void)
{
    GPIO_InitTypeDef gpioInit;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &gpioInit);
    // must set the pin high
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}
/**
 * @brief function to write to a register on the RFID
 *
 * @param reg the register to write to
 * @param value the value to write to the register
 */
void rfid_write_reg(uint8_t reg, uint8_t value)
{
    RFID_CS_LOW();
    rfid_send_byte((reg << 1) & 0x7E);
    rfid_send_byte(value);
    RFID_CS_HIGH();
}

/**
 * @brief function to read from a register on the RFID
 *
 * @param reg the register to read from
 * @return uint8_t the value read from the register
 */
uint8_t rfid_read_reg(uint8_t reg)
{
    uint8_t result;
    RFID_CS_LOW();
    rfid_send_byte(((reg << 1) & 0x7E) | 0x80);
    result = spi_send_byte(MFRC522_DUMMY);
    RFID_CS_HIGH();
    return result;
}
/**
 * @brief function to receive a byte from the RFID
 *
 * @return uint8_t
 */
uint8_t rfid_receive_byte(void)
{
    return spi_send_byte(MFRC522_DUMMY);
}

/**
 * @brief function to send a byte to the RFID
 *
 * @param byte the byte to send
 * @return uint8_t the byte received
 */
uint8_t rfid_send_byte(uint8_t byte)
{
    return spi_send_byte(byte);
}

/**
 * @brief initialize the RFID module
 *
 */
void rfid_init(void)
{
    spi_config();
    pin_cs_init();

    // Reset MFRC522
    rfid_write_reg(MFRC522_REG_COMMAND, PCD_RESETPHASE);

    // Set Timer
    rfid_write_reg(MFRC522_REG_T_MODE, 0x8D);
    rfid_write_reg(MFRC522_REG_T_PRESCALER, 0x3E);
    rfid_write_reg(MFRC522_REG_T_RELOAD_L, 30);
    rfid_write_reg(MFRC522_REG_T_RELOAD_H, 0);

    // Set TAuto=0x84
    rfid_write_reg(MFRC522_REG_RF_CFG, 0x70);
    // Set TxAuto=0x40
    rfid_write_reg(MFRC522_REG_TX_AUTO, 0x40);
    rfid_write_reg(MFRC522_REG_MODE, 0x3D);

    rfid_antenna_on();
}

/**
 * @brief  Set the bits of a register
 *
 * @param reg
 * @param mask
 */
void rfid_set_bit_mask(uint8_t reg, uint8_t mask)
{
    rfid_write_reg(reg, rfid_read_reg(reg) | mask);
}

/**
 * @brief clear the bits of a register
 *
 * @param reg
 * @param mask
 */
void rfid_clear_bit_mask(uint8_t reg, uint8_t mask)
{
    rfid_write_reg(reg, rfid_read_reg(reg) & ~mask);
}

/**
 * @brief turn on the RFID antenna
 *
 */
void rfid_antenna_on(void)
{
    uint8_t value = rfid_read_reg(MFRC522_REG_TX_CONTROL);
    if ((value & 0x03) != 0x03)
    {
        rfid_set_bit_mask(MFRC522_REG_TX_CONTROL, value | 0x03);
    }
}

/**
 * @brief turn off the RFID antenna
 *
 */
void rfid_antenna_off(void)
{
    rfid_clear_bit_mask(MFRC522_REG_TX_CONTROL, 0x03);
}

/**
 * @brief function to select the RFID
 *
 * @param buffer
 * @param length
 */
void rfid_read(uint8_t *buffer, uint8_t length)
{
    uint8_t i;

    if (length == 0)
        return;

    RFID_CS_LOW();

    for (i = 0; i < length; i++)
    {
        buffer[i] = rfid_receive_byte();
    }

    RFID_CS_HIGH();
}

/**
 * @brief function to write to the RFID
 *
 * @param buffer
 * @param length
 */
void rfid_write(uint8_t *buffer, uint8_t length)
{
    uint8_t i;

    if (length == 0)
        return;

    RFID_CS_LOW();

    for (i = 0; i < length; i++)
    {
        rfid_send_byte(buffer[i]);
    }
    RFID_CS_HIGH();
}

/**
 * @brief function to send a command to the RFID
 *
 * @param command
 * @param sendData
 * @param sendLen
 * @param backData
 * @param backLen
 * @return uint8_t
 */
uint8_t rfid_to_card(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen)
{
    uint8_t status = 0;
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    int i;

    switch (command)
    {
    case PCD_AUTHENT:
        irqEn = 0x12;
        waitIRq = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn = 0x77;
        waitIRq = 0x30;
        break;
    default:
        break;
    }

    rfid_write_reg(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
    rfid_clear_bit_mask(MFRC522_REG_COMM_IRQ, 0x80);
    rfid_set_bit_mask(MFRC522_REG_FIFO_LEVEL, 0x80);

    rfid_write_reg(MFRC522_REG_COMMAND, PCD_IDLE);

    // Write data to FIFO
    for (i = 0; i < sendLen; i++)
    {
        rfid_write_reg(MFRC522_REG_FIFO_DATA, sendData[i]);
    }

    // Execute command
    rfid_write_reg(MFRC522_REG_COMMAND, command);
    if (command == PCD_TRANSCEIVE)
    {
        rfid_set_bit_mask(MFRC522_REG_BIT_FRAMING, 0x80);
    }

    // Wait for command execution to complete
    i = 2000; // Max wait time
    do
    {
        n = rfid_read_reg(MFRC522_REG_COMM_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    rfid_clear_bit_mask(MFRC522_REG_BIT_FRAMING, 0x80);

    if (i != 0)
    {
        if (!(rfid_read_reg(MFRC522_REG_ERROR) & 0x1B))
        {
            status = 1; // Success

            if (n & irqEn & 0x01)
            {
                status = 0; // Error - no card detected
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = rfid_read_reg(MFRC522_REG_FIFO_LEVEL);
                lastBits = rfid_read_reg(MFRC522_REG_CONTROL) & 0x07;
                if (lastBits)
                {
                    *backLen = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *backLen = n * 8;
                }

                if (n == 0)
                {
                    n = 1;
                }
                if (n > MFRC522_MAX_LEN)
                {
                    n = MFRC522_MAX_LEN;
                }

                // Read received data from FIFO
                for (i = 0; i < n; i++)
                {
                    backData[i] = rfid_read_reg(MFRC522_REG_FIFO_DATA);
                }
            }
        }
        else
        {
            status = 0; // Error
        }
    }

    return status;
}

/**
 * @brief function to send a request to the RFID
 *
 * @param reqMode
 * @param TagType
 * @return uint8_t
 */
uint8_t rfid_request(uint8_t reqMode, uint8_t *TagType)
{
    uint8_t status;
    uint8_t backBits;
    uint8_t buffer[MFRC522_MAX_LEN];

    rfid_write_reg(MFRC522_REG_BIT_FRAMING, 0x07); // TxLastBits = BitFramingReg[2..0]

    buffer[0] = reqMode;
    status = rfid_to_card(PCD_TRANSCEIVE, buffer, 1, buffer, &backBits);

    if ((status == 1) && (backBits == 0x10))
    {
        *TagType = buffer[0];
        *(TagType + 1) = buffer[1];
    }
    else
    {
        status = 0;
    }

    return status;
}

/**
 * @brief function to perform anticollision on the RFID
 *
 * @param serNum
 * @return uint8_t
 */
uint8_t rfid_anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint8_t backBits;
    uint8_t buffer[MFRC522_MAX_LEN];

    rfid_write_reg(MFRC522_REG_BIT_FRAMING, 0x00);

    buffer[0] = PICC_ANTICOLL;
    buffer[1] = 0x20;
    status = rfid_to_card(PCD_TRANSCEIVE, buffer, 2, buffer, &backBits);

    if (status == 1)
    {
        // Check card serial number
        for (i = 0; i < 4; i++)
        {
            serNumCheck ^= buffer[i];
            serNum[i] = buffer[i];
        }
        if (serNumCheck != buffer[4])
        {
            status = 0;
        }
    }

    return status;
}

/**
 * @brief function to halt the RFID
 *
 */
void rfid_halt(void)
{
    uint8_t buffer[4];
    uint8_t backLen;

    buffer[0] = PICC_HALT;
    buffer[1] = 0;

    // Calculate CRC_A
    buffer[2] = 0;
    buffer[3] = 0;

    rfid_to_card(PCD_TRANSCEIVE, buffer, 4, buffer, &backLen);
}

/**
 * @brief function to read a card from the RFID
 *
 */
void rfid_debug_read_card(void)
{
    uint8_t status;
    uint8_t tagType[2];
    uint8_t serialNum[5];
    uint8_t i;

    // Reset variables
    for (i = 0; i < 5; i++)
    {
        serialNum[i] = 0;
    }

    // Find cards
    status = rfid_request(PICC_REQIDL, tagType);
    if (status == 1)
    {
        usart_send_string("Card detected! Type: ");
        usart_send_hex(tagType[0]);
        usart_send_hex(tagType[1]);
        usart_send_string("\r\n");

        // Get card serial number
        status = rfid_anticoll(serialNum);
        if (status == 1)
        {
            usart_send_string("Card UID: ");
            for (i = 0; i < 4; i++)
            {
                usart_send_hex(serialNum[i]);
                usart_send_char(' ');
            }
            usart_send_string("\r\n");
        }
        else
        {
            usart_send_string("Anticoll error\r\n");
        }

        rfid_halt(); // Halt PICC
    }
}
