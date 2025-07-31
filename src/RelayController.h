#pragma once
#include <Arduino.h>
#include "Config.h"
#include "RelyWarm.h"
#include "RelyRain.h"

class RelayController
{
public:
    void setup()
    {
        pinMode(RELAY_PIN_LIGHT, OUTPUT);
        pinMode(RELAY_PIN_RAIN, OUTPUT);
        pinMode(RELAY_PIN_WARM, OUTPUT);
        setLight(false);
        setRain(false);
        setWarm(false);
    }

    void setLight(bool state)
    {
        digitalWrite(RELAY_PIN_LIGHT, state);
        lightState = state;
    }

    void setRain(bool state)
    {
        digitalWrite(RELAY_PIN_RAIN, state);
        rainState = state;
    }

    void setWarm(bool state)
    {
        digitalWrite(RELAY_PIN_WARM, state);
        warmState = state;
    }

    void updateRain()
    {
        setRain(releRain(RAIN_PERIOD, RAIN_WORK_TIME));
    }

    void updateWarm(float temperature)
    {
        setWarm(releWarm(temperature, SETPOINT_TEMP));
    }

    bool getLightState() const { return lightState; }
    bool getRainState() const { return rainState; }
    bool getWarmState() const { return warmState; }

private:
    bool lightState = false;
    bool rainState = false;
    bool warmState = false;
};