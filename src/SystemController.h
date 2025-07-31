#pragma once
#include <GyverNTP.h>
#include "Config.h"
#include "SensorDHT.h"
#include "Fan.h"
#include "GyverPID.h"
#include "RelayController.h"
#include "MqttHandler.h"



class SystemController
{
    

public:
    
    SystemController() : sensor(DHT_PIN),
                         fan(Fan_Pin_PWM, Fan_STOP),
                         regulator(25, 0.5, 0, 10) {}

    void setup()
    {
        // Настройка компонентов
        sensor.sensor_init();
        regulator.setDirection(REVERSE);
        regulator.setLimits(0, 100);
        regulator.setpoint = SETPOINT_TEMP;

        // Настройка пинов
        digitalWrite(Fan_STOP, 0);
        pinMode(Fan_Pin_Tach, INPUT_PULLUP);
        
        // attachInterrupt(digitalPinToInterrupt(Fan_Pin_Tach), HandleInterrupt, FALLING);

        // Настройка NTP
        NTP.onError([]()
                    {
      Serial.print("NTP error: ");
    //   Serial.println(NTP.readError()); 
        });
        NTP.begin(3);
    }

    void loop()
    {
        NTP.tick();
        mqtt.loop();

        handleOneSecondTasks();
        handleFanControl();
        handleLighting();
    }

private:
    SensorDHT sensor;
    Fan fan;
    GyverPID regulator;
    RelayController relays;
    MqttHandler mqtt;
    GyverNTP NTP;

    uint32_t timer = 0;
    uint32_t timer_fan = 0;
    float lastTemperature = 0;

    void ICACHE_RAM_ATTR HandleInterrupt()
    {
        fan.tick();
    };

    void handleOneSecondTasks()
    {
        if (millis() - timer >= PERIOD_SECOND)
        {
            timer = millis();

            auto data = sensor.get_DHT();
            lastTemperature = data.Temperature;

            relays.updateRain();
            relays.updateWarm(data.Temperature);

            sendMqttData(data);
            printSerialData(data);
        }
    }

    void handleFanControl()
    {
        if (millis() - timer_fan >= FAN_PERIOD)
        {
            timer_fan = millis();
            regulator.input = sensor.get_DHT().Temperature;
            fan.SetFanLevel(regulator.getResultTimer());
        }
    }

    void handleLighting()
    {
        uint32_t now = NTP.daySeconds();
        uint32_t start = 21 * 3600 + 18 * 60; // 21:18
        uint32_t stop = start + WORK_TIME;

        bool shouldBeOn = (now >= start && now < stop);
        if (shouldBeOn != relays.getLightState())
        {
            relays.setLight(shouldBeOn);
        }
    }

    void sendMqttData(const SensorDHT::Struct_DHT_Val &data)
    {
        if (abs(data.Temperature - lastTemperature) >= 1.0)
        {
            mqtt.sendData(
                NTP.toString(),
                data.Temperature,
                data.Humidity,
                fan.getRPM(),
                regulator.getResultTimer(),
                relays.getLightState(),
                relays.getRainState(),
                relays.getWarmState());
            lastTemperature = data.Temperature;
        }
    }

    void printSerialData(const SensorDHT::Struct_DHT_Val &data)
    {
        Serial.print(NTP.toString());
        Serial.print(" T:");
        Serial.print(data.Temperature);
        Serial.print(" H:");
        Serial.print(data.Humidity);
        Serial.print(" Fan:");
        Serial.print(fan.getRPM());
        Serial.print(" Reg:");
        Serial.print(regulator.getResultTimer());
        Serial.print(" Light:");
        Serial.print(relays.getLightState());
        Serial.print(" Rain:");
        Serial.print(relays.getRainState());
        Serial.print(" Warm:");
        Serial.println(relays.getWarmState());
    }
};