
#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "thang"
#define WIFI_PASSWORD "11122004"
#define TOPIC_SUB "face_detect"
#define TOPIC_PUB "face_detect"
#define MQTT_PORT 1883
#define MQTT_SERVER "broker.hivemq.com"

class MQTTClient {
public:
	MQTTClient();
	void wifiConnect();
	void mqttConnect();
	void mqttReconnect();
	void mqttPublish(const char* topic, const char* message);
	String getMessage();
	void loop();

private:
	WiFiClient espClient;
	PubSubClient client;
	String message;
	static void callback(char* topic, byte* payload, unsigned int length);
	static MQTTClient* instance;
};

#endif