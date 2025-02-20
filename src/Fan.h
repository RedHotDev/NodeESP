#pragma once
#include <Arduino.h>

#define Fan_PWM_FRQ 25000

#define Fan_RPM_Update_Period 1000





class Fan
{
private:
bool getTime() {
  if (ticks > window) {
      tmrMs = millis();
      prd = (uint32_t)tmr / window;
      ticks = 0;
  }
  return (millis() - tmrMs < tout && prd);
}
  volatile uint32_t tmr;
  volatile uint8_t ticks = 0;
  byte _fan_pin_PWM;
  byte _fan_pin_Stop;
  uint8_t window = 10;
  uint32_t tmrMs = millis();
  uint32_t prd = 0;
  uint16_t tout = 100;

  
public:
  Fan(byte fan_pin_PWM, byte fan_pin_Stop) {
    _fan_pin_PWM = fan_pin_PWM;
    _fan_pin_Stop=fan_pin_Stop; 
    analogWriteFreq(Fan_PWM_FRQ);
    pinMode(_fan_pin_PWM, OUTPUT);
    pinMode(_fan_pin_Stop, OUTPUT);
  };

// тикер - вызывать в прерывании по фронту
void tick() {
  if (ticks <= window) {
      if (!ticks) tmr = micros();
      if (ticks == window) tmr = micros() - tmr;
      ticks++;
  }
}

  void SetFanLevel(int percent) {
    if (percent == 0) {
      digitalWrite(_fan_pin_Stop, 0);
     return;
    }
    digitalWrite(_fan_pin_Stop, 1);
    analogWrite(_fan_pin_PWM,  map(percent, 0, 100, 0, 256));
  };

     // получить обороты в минуту
     uint32_t getRPM() {
      return getTime() ? (60000000ul / prd) : 0;
  };

};

