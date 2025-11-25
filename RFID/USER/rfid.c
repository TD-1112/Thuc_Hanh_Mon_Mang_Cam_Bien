#include "rfid.h"

// ========== Internal (Private) Function Declarations ==========
static void pin_cs_init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
static uint8_t rfid_send_byte(uint8_t byte);
static uint8_t rfid_receive_byte(void);
static uint8_t rfid_read_reg(uint8_t reg);
static void rfid_write_reg(uint8_t reg, uint8_t value);
static void rfid_read(uint8_t *buffer, uint8_t length);
static void rfid_write(uint8_t *buffer, uint8_t length);
static void rfid_set_bit_mask(uint8_t reg, uint8_t mask);
static void rfid_clear_bit_mask(uint8_t reg, uint8_t mask);
static void rfid_antenna_on(void);
static void rfid_antenna_off(void);
static uint8_t rfid_to_card(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen);
static uint8_t rfid_anticoll(uint8_t *serNum);
static uint8_t rfid_request(uint8_t reqMode, uint8_t *TagType);
static void rfid_calculate_crc(uint8_t *data, uint8_t len, uint8_t *result);
static void rfid_halt(void);

// ========== Internal (Private) Function Implementations ==========

/**
 * @brief function to initialize pin cs for RFID (pin 12 on port B)
 *
 */
static void pin_cs_init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef gpioInit;
    if (GPIOx == GPIOB)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInit.GPIO_Pin = GPIO_Pin;
    ;
    GPIO_Init(GPIOx, &gpioInit);
    // must set the pin high
    GPIO_SetBits(GPIOx, GPIO_Pin);
}
/**
 * @brief function to write to a register on the RFID
 *
 * @param reg the register to write to
 * @param value the value to write to the register
 */
static void rfid_write_reg(uint8_t reg, uint8_t value)
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
static uint8_t rfid_read_reg(uint8_t reg)
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
static uint8_t rfid_receive_byte(void)
{
    return spi_send_byte(MFRC522_DUMMY);
}

/**
 * @brief function to send a byte to the RFID
 *
 * @param byte the byte to send
 * @return uint8_t the byte received
 */
static uint8_t rfid_send_byte(uint8_t byte)
{
    return spi_send_byte(byte);
}

/**
 * @brief initialize the RFID module
 *
 */
void rfid_init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    spi_config();
    pin_cs_init(GPIOx, GPIO_Pin);

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
static void rfid_set_bit_mask(uint8_t reg, uint8_t mask)
{
    rfid_write_reg(reg, rfid_read_reg(reg) | mask);
}

/**
 * @brief clear the bits of a register
 *
 * @param reg
 * @param mask
 */
static void rfid_clear_bit_mask(uint8_t reg, uint8_t mask)
{
    rfid_write_reg(reg, rfid_read_reg(reg) & ~mask);
}

/**
 * @brief turn on the RFID antenna
 *
 */
static void rfid_antenna_on(void)
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
static void rfid_antenna_off(void)
{
    rfid_clear_bit_mask(MFRC522_REG_TX_CONTROL, 0x03);
}

/**
 * @brief function to select the RFID
 *
 * @param buffer
 * @param length
 */
static void rfid_read(uint8_t *buffer, uint8_t length)
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
static void rfid_write(uint8_t *buffer, uint8_t length)
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
static uint8_t rfid_to_card(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen)
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
static uint8_t rfid_request(uint8_t reqMode, uint8_t *TagType)
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
static uint8_t rfid_anticoll(uint8_t *serNum)
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
 * @brief Calculate CRC using MFRC522 CRC coprocessor
 *
 * @param data Pointer to data buffer
 * @param len Length of data
 * @param result Pointer to store 2-byte CRC result
 */
static void rfid_calculate_crc(uint8_t *data, uint8_t len, uint8_t *result)
{
    uint8_t i;
    uint8_t n;

    // Clear CRCIRq interrupt flag
    rfid_clear_bit_mask(MFRC522_REG_DIV_IRQ, 0x04);
    // Flush FIFO buffer
    rfid_set_bit_mask(MFRC522_REG_FIFO_LEVEL, 0x80);

    // Write data to FIFO
    for (i = 0; i < len; i++)
    {
        rfid_write_reg(MFRC522_REG_FIFO_DATA, data[i]);
    }

    // Start CRC calculation
    rfid_write_reg(MFRC522_REG_COMMAND, PCD_CALCCRC);

    // Wait for CRC calculation to complete
    i = 0xFF;
    do
    {
        n = rfid_read_reg(MFRC522_REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04));

    // Read CRC result (LSB first, then MSB)
    result[0] = rfid_read_reg(MFRC522_REG_CRC_RESULT_L);
    result[1] = rfid_read_reg(MFRC522_REG_CRC_RESULT_M);
}

/**
 * @brief function to halt the RFID
 *
 */
static void rfid_halt(void)
{
    uint8_t buffer[4];
    uint8_t backLen;

    buffer[0] = PICC_HALT;
    buffer[1] = 0;

    // Calculate CRC_A using MFRC522 coprocessor
    rfid_calculate_crc(buffer, 2, &buffer[2]);

    rfid_to_card(PCD_TRANSCEIVE, buffer, 4, buffer, &backLen);
}

/**
 * @brief Read RFID card data
 *
 * @return RFID_CardData structure containing UID, tag type, and status
 */
RFID_CardData rfid_read_card(void)
{
    RFID_CardData cardData;
    uint8_t status;
    uint8_t serialNum[5];
    uint8_t i;

    cardData.status = 0;
    for (i = 0; i < 4; i++)
    {
        cardData.uid[i] = 0;
    }
    cardData.tagType[0] = 0;
    cardData.tagType[1] = 0;

    for (i = 0; i < 5; i++)
    {
        serialNum[i] = 0;
    }

    // Find cards
    status = rfid_request(PICC_REQIDL, cardData.tagType);
    if (status == 1)
    {
        // Get card serial number
        status = rfid_anticoll(serialNum);
        if (status == 1)
        {
            // Copy UID (first 4 bytes)
            for (i = 0; i < 4; i++)
            {
                cardData.uid[i] = serialNum[i];
            }
            cardData.status = 1;
        }

        rfid_halt();
    }

    return cardData;
}

/**
 * @brief Initialize card list
 *
 * @param list Pointer to RFID_CardList structure
 */
void rfid_card_list_init(RFID_CardList *list)
{
    uint8_t i, j;

    if (list == 0)
        return;

    list->count = 0;

    // Clear all card data
    for (i = 0; i < MAX_CARDS; i++)
    {
        list->cards[i].status = 0;
        for (j = 0; j < 4; j++)
        {
            list->cards[i].uid[j] = 0;
        }
        list->cards[i].tagType[0] = 0;
        list->cards[i].tagType[1] = 0;
    }
}

/**
 * @brief Check if UID already exists in the card list
 *
 * @param list Pointer to RFID_CardList structure
 * @param uid Pointer to UID array (4 bytes)
 * @return uint8_t 1 if UID exists, 0 if not found
 */
uint8_t rfid_check_uid_exists(RFID_CardList *list, uint8_t *uid)
{
    uint8_t i, j;
    uint8_t match;

    if (list == 0 || uid == 0)
        return 0;

    // Check each stored card
    for (i = 0; i < list->count; i++)
    {
        match = 1;
        // Compare all 4 bytes of UID
        for (j = 0; j < 4; j++)
        {
            if (list->cards[i].uid[j] != uid[j])
            {
                match = 0;
                break;
            }
        }

        if (match == 1)
            return 1; // UID found
    }

    return 0; // UID not found
}

/**
 * @brief Add a card to the list if it doesn't already exist
 *
 * @param list Pointer to RFID_CardList structure
 * @param card Pointer to RFID_CardData to add
 * @return uint8_t 1 if added successfully, 0 if failed (list full or UID exists)
 */
uint8_t rfid_add_card(RFID_CardList *list, RFID_CardData *card)
{
    uint8_t i;

    if (list == 0 || card == 0)
        return 0;

    // Check if list is full
    if (list->count >= MAX_CARDS)
        return 0;

    // Check if UID already exists
    if (rfid_check_uid_exists(list, card->uid) == 1)
        return 0;

    // Add card to list
    for (i = 0; i < 4; i++)
    {
        list->cards[list->count].uid[i] = card->uid[i];
    }
    list->cards[list->count].tagType[0] = card->tagType[0];
    list->cards[list->count].tagType[1] = card->tagType[1];
    list->cards[list->count].status = card->status;

    list->count++;

    return 1; // Successfully added
}

/**
 * @brief Remove a card from the list by UID
 *
 * @param list Pointer to RFID_CardList structure
 * @param uid Pointer to UID array (4 bytes)
 * @return uint8_t 1 if removed successfully, 0 if UID not found
 */
uint8_t rfid_remove_card(RFID_CardList *list, uint8_t *uid)
{
    uint8_t i, j, k;
    uint8_t match;

    if (list == 0 || uid == 0)
    {
        return 0;
    }
    // Find the card with matching UID
    for (i = 0; i < list->count; i++)
    {
        match = 1;
        for (j = 0; j < 4; j++)
        {
            if (list->cards[i].uid[j] != uid[j])
            {
                match = 0;
                break;
            }
        }

        if (match == 1)
        {
            // Shift all cards after this one down
            for (k = i; k < list->count - 1; k++)
            {
                for (j = 0; j < 4; j++)
                {
                    list->cards[k].uid[j] = list->cards[k + 1].uid[j];
                }
                list->cards[k].tagType[0] = list->cards[k + 1].tagType[0];
                list->cards[k].tagType[1] = list->cards[k + 1].tagType[1];
                list->cards[k].status = list->cards[k + 1].status;
            }

            list->count--;
            return 1; // Successfully removed
        }
    }

    return 0; // UID not found
}

/**
 * @brief Clear all cards from the list
 *
 * @param list Pointer to RFID_CardList structure
 */
void rfid_clear_card_list(RFID_CardList *list)
{
    rfid_card_list_init(list);
}

/**
 * @brief Get the number of cards in the list
 *
 * @param list Pointer to RFID_CardList structure
 * @return uint8_t Number of cards stored
 */
uint8_t rfid_get_card_count(RFID_CardList *list)
{
    if (list == 0)
    {
        return 0;
    }
    return list->count;
}

/**
 * @brief Get a card by index
 *
 * @param list Pointer to RFID_CardList structure
 * @param index Index of the card (0 to count-1)
 * @return RFID_CardData* Pointer to card data, or 0 if invalid index
 */
RFID_CardData *rfid_get_card_by_index(RFID_CardList *list, uint8_t index)
{
    if (list == 0 || index >= list->count)
    {
        return 0;
    }
    return &list->cards[index];
}



RFID_Handler RFID;

// Wrapper functions to access cardList from RFID global
static uint8_t rfid_handler_check_uid_wrapper(uint8_t *uid)
{
    return rfid_check_uid_exists(&RFID.cardList, uid);
}

static uint8_t rfid_handler_add_card_wrapper(RFID_CardData *card)
{
    return rfid_add_card(&RFID.cardList, card);
}

static uint8_t rfid_handler_remove_card_wrapper(uint8_t *uid)
{
    return rfid_remove_card(&RFID.cardList, uid);
}

static void rfid_handler_clear_list_wrapper(void)
{
    rfid_clear_card_list(&RFID.cardList);
}

static uint8_t rfid_handler_get_count_wrapper(void)
{
    return rfid_get_card_count(&RFID.cardList);
}

static RFID_CardData *rfid_handler_get_by_index_wrapper(uint8_t index)
{
    return rfid_get_card_by_index(&RFID.cardList, index);
}

/**
 * @brief Initialize RFID Handler with function pointers
 * @note This function runs automatically before main() using __attribute__((constructor))
 */
void RFID_FirstInit(void) __attribute__((constructor));

void RFID_FirstInit(void)
{
    // Initialize card list
    rfid_card_list_init(&RFID.cardList);

    // Bind function pointers
    RFID.init = rfid_init;
    RFID.read_card = rfid_read_card;
    RFID.check_uid_exists = rfid_handler_check_uid_wrapper;
    RFID.add_card = rfid_handler_add_card_wrapper;
    RFID.remove_card = rfid_handler_remove_card_wrapper;
    RFID.clear_list = rfid_handler_clear_list_wrapper;
    RFID.get_card_count = rfid_handler_get_count_wrapper;
    RFID.get_card_by_index = rfid_handler_get_by_index_wrapper;
}
