#include "SysTick.h"
#include "lib_gpio.h"
#include "lib_uart.h"
#include "rfid.h"
#include "message.h"

int main(void)
{
  systick_init();
  UART.init(USART1, 115200, DEFAULT);
  RFID.init(GPIOB, GPIO_Pin_12);
  UART.send_string("RFID Reader Ready\r\n");
  const uint8_t CARD_INFO_UID[] = {0x52, 0x6B, 0x79, 0x5C};
  const uint8_t CARD_LOC_UID[] = {0xC5, 0xA5, 0xF5, 0x05};

  while (1)
  {
    RFID_CardData cardData = RFID.read_card();
    if (cardData.status)
    {
      uint8_t buffer[100];
      uint16_t len = 0;

      if (memcmp(cardData.uid, CARD_INFO_UID, 4) == 0)
      {
        Data_Infor info;
        memcpy(info.uid, cardData.uid, 4);
        info.product_code = PRODUCT_SP1;
        info.status = STATUS_VALID;

        len = create_data_infor_message(&info, buffer);
      }
      else if (memcmp(cardData.uid, CARD_LOC_UID, 4) == 0)
      {
        Data_Location loc;
        // Example Location: Region 10, Row 11, Shelf 12
        loc.region = 10;
        loc.row = 11;
        loc.shelf = 12;

        len = create_data_location_message(&loc, buffer);
      }
      if (len > 0)
      {
        UART.send_data(buffer, len);
        delay_ms(1000);
      }
    }
  }
}


