
#include "mqtt.h"

MQTTClient* MQTTClient::instance = nullptr;

MQTTClient::MQTTClient() : client(espClient), message("") {
    instance = this;
}

void MQTTClient::wifiConnect() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void MQTTClient::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    if (instance) {
        instance->message = "";
        for (unsigned int i = 0; i < length; i++) {
            instance->message += (char)payload[i];
        }
        Serial.println("Message: " + instance->message);
    }
}

void MQTTClient::mqttConnect() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(MQTTClient::callback);
    while (!client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT broker with client ID: %s\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
            Serial.println("MQTT connected");
            client.subscribe(TOPIC_SUB);
        } else {
            Serial.print("MQTT connection failed, state: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void MQTTClient::mqttReconnect() {
    if (!client.connected()) {
        Serial.println("Attempting MQTT reconnection...");
        if (client.connect("ESP32Client")) {
            Serial.println("MQTT reconnected");
            client.subscribe(TOPIC_SUB);
        } else {
            Serial.print("Reconnect failed, state: ");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

void MQTTClient::mqttPublish(const char* topic, const char* msg) {
    client.publish(topic, msg);
}

String MQTTClient::getMessage() {
    return message;
}

void MQTTClient::loop() {
    client.loop();
}