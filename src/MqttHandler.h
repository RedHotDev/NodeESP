#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "SensorDHT.h"
#include "Fan.h"
#include "GyverPID.h"

class MqttHandler
{
public:
    void setup()
    {
        client.setServer(MQTT_SERVER, MQTT_PORT);
        reconnect();
    }

    void loop()
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
    }

    void sendData(
        const String &dateTime,
        float temperature,
        float humidity,
        uint32_t fanRpm,
        float fanReg,
        bool lightState,
        bool rainState,
        bool warmState)
    {
        JsonDocument doc;
        doc["DateStamp"] = dateTime;
        doc["T"] = temperature;
        doc["H"] = humidity;
        doc["Fan"] = fanRpm;
        doc["FanReg"] = fanReg;
        doc["Light"] = lightState;
        doc["Rain"] = rainState;
        doc["Warm"] = warmState;

        String json;
        serializeJson(doc, json);
        client.publish(MQTT_TOPIC, json.c_str());
    }

private:
    WiFiClient espClient;
    PubSubClient client = PubSubClient(espClient);

    void reconnect()
    {
        while (!client.connected())
        {
            if (client.connect(MQTT_CLIENT_NAME))
            {
                Serial.println("MQTT connected");
            }
            else
            {
                Serial.print("MQTT failed, rc=");
                Serial.println(client.state());
                delay(5000);
            }
        }
    }
};