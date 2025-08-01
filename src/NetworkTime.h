#pragma once
#include <ESP8266WiFi.h>
#include <GyverNTP.h>
#include "Config.h"

class NetworkTime
{
public:
    NetworkTime() : ntp() {}

    void begin()
    {
        WiFi.begin(WLAN_SSID, WLAN_PASS);
        while (WiFi.status() != WL_CONNECTED)
            delay(500);

        ntp.onError([]()
                    {
            Serial.print("NTP error: ");
            // Serial.println(ntp.readError());
             });

        ntp.begin(3);
    }

    void tick()
    {
        ntp.tick();
    }

    uint32_t daySeconds()
    {
        return ntp.daySeconds();
    }

    String toString()
    {
        return ntp.toString();
    }

private:
    GyverNTP ntp;
};