#include "mqtt.h"

MQTTClient mqtt;


void setup()
{
  Serial.begin(115200);
  mqtt.wifiConnect();
  mqtt.mqttConnect();
}


void loop()
{
  mqtt.loop();

}
