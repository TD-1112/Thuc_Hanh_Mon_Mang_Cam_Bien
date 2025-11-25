#include "mqtt.h"

MQTTClient *MQTTClient::instance = nullptr;

/**
 * @brief function to encode MQTT message to JSON
 *
 * @param msg
 * @return String
 */
String MQTTClient::encodeMQTTMessage(const MQTT_Message &msg)
{
    StaticJsonDocument<512> doc;
    doc["id"] = msg.id;
    doc["dst"] = msg.dst;
    doc["cmd"] = msg.cmd;
    doc["data"] = msg.data;

    String output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief function to decode MQTT message from JSON
 *
 * @param json
 * @param msg
 * @return true
 * @return false
 */
bool MQTTClient::decodeMQTTMessage(const String &json, MQTT_Message &msg)
{
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        return false;
    }

    msg.id = doc["id"] | "";
    msg.dst = doc["dst"] | "";
    msg.cmd = doc["cmd"] | "";

    msg.data.clear();
    msg.data.set(doc["data"]);

    return true;
}

/**
 * @brief function to send MQTT message
 * 
 * @param src 
 * @param dst 
 * @param cmd 
 * @param fillData 
 */
void MQTTClient::sendMQTTMessage(const String &src,
                                 const String &dst,
                                 const String &cmd,
                                 std::function<void(JsonDocument &)> fillData)
{
    MQTT_Message msg;
    msg.id = src;
    msg.dst = dst;
    msg.cmd = cmd;

    fillData(msg.data);

    String payload = encodeMQTTMessage(msg);
    mqttPublish(config.topic_pub, payload.c_str());
}

/**
 * @brief Construct a new MQTTClient::MQTTClient object 
 * 
 * @param cfg 
 */
MQTTClient::MQTTClient(const MQTT_Config &cfg)
    : client(espClient), message(""), config(cfg), useBackupBroker(false)
{
    instance = this;
}

/**
 * @brief Connect to WiFi
 * 
 * @param timeoutMs 
 */
void MQTTClient::wifiConnect(unsigned long timeoutMs)
{
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    unsigned long startAttempt = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < timeoutMs)
    {
        delay(1000);
        Serial.println("[WIFI] Connecting to WiFi...");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[WIFI] Connected to WiFi");
        Serial.print("[WIFI] IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("[WIFI] WiFi connection failed (timeout).");
    }
}

/**
 * @brief Callback function for MQTT messages
 * 
 * @param topic 
 * @param payload 
 * @param length 
 */
void MQTTClient::callback(char *topic, byte *payload, unsigned int length)
{
    if (instance)
    {
        instance->message = "";
        for (unsigned int i = 0; i < length; i++)
        {
            instance->message += (char)payload[i];
        }
        // Serial.printf("[MQTT CALLBACK] Message arrived [%s]: %s\n", topic, instance->message.c_str());
    }
}

/**
 * @brief Connect to MQTT broker
 * 
 */
void MQTTClient::mqttConnect()
{
    client.setServer(config.server, config.port);
    client.setCallback(MQTTClient::callback);
    client.setKeepAlive(60);
    client.setBufferSize(512);

    int attempt = 0;
    const int MAX_PRIMARY_ATTEMPTS = 3;
    bool useBackupBroker = false;

    while (!client.connected())
    {
        String client_id = "esp32-client-" + String(WiFi.macAddress());

        // Auto-fallback to backup broker after failed attempts
        if (attempt >= MAX_PRIMARY_ATTEMPTS && !useBackupBroker)
        {
            useBackupBroker = true;
            client.setServer("test.mosquitto.org", 1883);
            Serial.println("[MQTT SETUP] Falling back to backup broker: test.mosquitto.org");
            attempt = 0;
        }

        String broker_name = useBackupBroker ? "backup (test.mosquitto.org)" : config.server;
        Serial.printf("[MQTT SETUP] Connecting to broker: %s [attempt %d]\n", broker_name.c_str(), attempt + 1);

        if (client.connect(client_id.c_str()))
        {
            Serial.printf("[MQTT SETUP] Connected to broker: %s\n", broker_name.c_str());
            client.subscribe(config.topic_sub);
            Serial.printf("[MQTT] Subscribed to: %s\n", config.topic_sub);
        }
        else
        {
            Serial.printf("[MQTT SETUP] Failed, state: %d - retrying...\n", client.state());
            attempt++;
            delay(2000);
        }
    }
}

/**
 * @brief Reconnect to MQTT broker
 * 
 */
void MQTTClient::mqttReconnect()
{
    if (!client.connected())
    {
        Serial.println("[MQTT] Attempting reconnection...");
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        if (client.connect(client_id.c_str()))
        {
            Serial.println("[MQTT] Reconnected successfully");
            client.subscribe(config.topic_sub);
            Serial.printf("[MQTT] Subscribed to: %s\n", config.topic_sub);
        }
        else
        {
            Serial.printf("[MQTT] Reconnect failed, state: %d\n", client.state());
            delay(5000);
        }
    }
}

/**
 * @brief Publish MQTT message
 * 
 * @param topic 
 * @param msg 
 */
void MQTTClient::mqttPublish(const char *topic, const char *msg)
{
    client.publish(topic, msg);
}

/**
 * @brief function to get the last received MQTT message
 * 
 * @return String 
 */
String MQTTClient::getMessage()
{
    String temp = message;
    message = "";
    return temp;
}

/**
 * @brief Loop function to maintain MQTT connection
 * 
 */
void MQTTClient::loop()
{
    client.loop();
}
