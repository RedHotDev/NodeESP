#pragma once
#include <Arduino.h>



static uint8_t s;
static bool ReleRainFlag;

bool releRain(uint32_t period_second, uint8_t work_raine) {
  s++;
  if ((s > period_second && !ReleRainFlag) || (s>work_raine && ReleRainFlag))  {
    s=0;
    ReleRainFlag = !ReleRainFlag;
    return ReleRainFlag;
   }
  return ReleRainFlag;
}