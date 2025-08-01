#pragma once
#include <Arduino.h>
#include "DHT.h"

class SensorDHT
{
public:
  SensorDHT(byte pin) : _pin(pin) {};

  struct Struct_DHT_Val
  {
    float Temperature;
    float Humidity;
  };

  void sensor_init()
  {
    dht.begin();
  }


  Struct_DHT_Val get_DHT()
  {
    static uint32_t tmr;
    if (millis() - tmr >= 1000)
    {
      tmr = millis();
      DHT_Val.Humidity = dht.readHumidity();       // Влажность
      DHT_Val.Temperature = dht.readTemperature(); // Температура
    }
    return DHT_Val;
  }

private:
  byte _pin;
  DHT dht = DHT(_pin, DHT22);
  Struct_DHT_Val DHT_Val;
};
