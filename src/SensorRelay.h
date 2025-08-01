#pragma once
#include "SensorDHT.h"
#include "RelyWarm.h"
#include "RelyRain.h"
#include "Config.h"
#include <Arduino.h>

class SensorRelay
{
public:
    SensorRelay() : dhtSensor(DHT_PIN) {}

    void begin()
    {
        pinMode(RELAY_WARM_PIN, OUTPUT);
        pinMode(RELAY_RAIN_PIN, OUTPUT);
        pinMode(RELAY_LIGHT_PIN, OUTPUT);
        dhtSensor.sensor_init();
    }

    void update()
    {
        auto data = dhtSensor.get_DHT();
        temperature = data.Temperature;
        humidity = data.Humidity;

        relayWarmState = releWarm(temperature, setPoint);
        relayRainState = releRain(period_second, work_raine);

        digitalWrite(RELAY_WARM_PIN, relayWarmState);
        digitalWrite(RELAY_RAIN_PIN, relayRainState);
    }

    void controlLight(uint32_t currentSecond)
    {
        bool newState = (currentSecond > start_second && currentSecond < stop_second);
        if (newState != relayLightState)
        {
            relayLightState = newState;
            digitalWrite(RELAY_LIGHT_PIN, relayLightState);
        }
    }

    float temperature = 0;
    float humidity = 0;
    bool relayWarmState = false;
    bool relayRainState = false;
    bool relayLightState = false;

private:
    SensorDHT dhtSensor;
};