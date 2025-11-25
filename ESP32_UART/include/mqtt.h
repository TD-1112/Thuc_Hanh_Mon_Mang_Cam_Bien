#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// ================== CONFIG ==================
struct MQTT_Config
{
    const char *wifi_ssid;
    const char *wifi_password;
    const char *topic_sub;
    const char *topic_pub;
    const char *server;
    uint16_t port;
};

// ================== MESSAGE ==================
struct MQTT_Message
{
    String id;
    String dst;
    String cmd;
    DynamicJsonDocument data;

    MQTT_Message(size_t capacity = 256) : data(capacity) {}
};

// ================== CLIENT ==================
class MQTTClient
{
public:
    MQTTClient(const MQTT_Config &cfg);
    void wifiConnect(unsigned long timeoutMs = 15000);
    void mqttConnect();
    void mqttReconnect();
    void mqttPublish(const char *topic, const char *message);
    String getMessage();
    // ================== ENCODE / DECODE ==================
    String encodeMQTTMessage(const MQTT_Message &msg);
    bool decodeMQTTMessage(const String &json, MQTT_Message &msg);
    // ================== MESSAGE ==================
    void sendMQTTMessage(const String &src,
                         const String &dst,
                         const String &cmd,
                         std::function<void(JsonDocument &)> fillData);
    void loop();

private:
    WiFiClient espClient;
    PubSubClient client;
    String message;
    MQTT_Config config;
    bool useBackupBroker;
    static void callback(char *topic, byte *payload, unsigned int length);
    static MQTTClient *instance;
};

#endif
