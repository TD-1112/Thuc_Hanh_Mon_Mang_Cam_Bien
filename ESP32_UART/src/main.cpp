#include "main.h"

MQTT_Config config = {
    .wifi_ssid = "thang",
    .wifi_password = "11122004",
    .topic_sub = "smart_scale/app",
    .topic_pub = "smart_scale/esp32",
    .server = "broker.emqx.io",
    .port = 1883};

MQTTClient mqtt(config);

uint8_t message_buffer[FSM_MAX_FRAME_SIZE];

void readUart2(void);

void setup()
{
  Serial.begin(115200);  // debug uart 0
  Serial2.begin(115200); // UART kết nối STM32 uart2
  mqtt.wifiConnect();
  mqtt.mqttConnect();
  Serial.println("ESP32 ready to receive from STM32...");
}

void loop()
{
  mqtt.loop();
  readUart2();

  uint16_t msg_length = 0;
  if (Is_Message(&msg_length))
  {
    Type_Message msg_type;
    uint16_t data_value;
    if (decode_message(message_buffer, msg_length, &msg_type, &data_value))
    {
      Serial.print("Message type: ");
      Serial.println(msg_type);
      Serial.print("Received value: ");
      Serial.println(data_value);
      mqtt.sendMQTTMessage("esp32", "app", "data", [msg_type, data_value](JsonDocument &doc)
                           {
        doc["type"] = msg_type;
        doc["value"] = data_value; });
    }
    else
    {
      Serial.println("Decode failed!");
    }
  }
}

void readUart2(void)
{
  while (Serial2.available())
  {
    uint8_t byte_in = Serial2.read();
    fsm_get_message(byte_in, message_buffer);
  }
}
