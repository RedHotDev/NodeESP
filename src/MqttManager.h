#pragma once
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "SensorRelay.h"

class MqttManager
{
public:
    MqttManager(WiFiClient &wifiClient) : client(wifiClient) {}

    void begin()
    {
        client.setServer(MQTT_SERVER, MQTT_PORT);
        if (client.connect("ESP", MQTT_USER, MQTT_PASS))
        {
            client.subscribe("CFG");
        }
    }

    void loop()
    {
        client.loop();
    }

    void publishData(SensorRelay &sensors, uint32_t rpm, uint8_t fanReg, const String &dateStamp)
    {
        JsonDocument doc;
        doc["t"] = sensors.temperature;
        doc["h"] = sensors.humidity;
        doc["fan"] = (rpm > 0);
        doc["fanspeed"] = rpm;
        doc["fanreg"] = fanReg;
        doc["light"] = sensors.relayLightState;
        doc["warm"] = sensors.relayWarmState;
        doc["rain"] = sensors.relayRainState;
        doc["dateStamp"] = dateStamp;

        String output;
        serializeJson(doc, output);
        client.publish("MAIN", output.c_str());
    }

    // Исправленная сигнатура
    void setCallback(MQTT_CALLBACK_SIGNATURE)
    {
        client.setCallback(callback);
    }

    bool isConnected()
    {
        return client.connected();
    }

    void reconnect()
    {
        if (!isConnected())
        {
            client.connect("ESP", MQTT_USER, MQTT_PASS);
        }
    }

private:
    PubSubClient client;
};