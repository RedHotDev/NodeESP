#pragma once
#include <Arduino.h>


#define HYSTER_WARM 1


bool releWarm(float temp , float setpoint) {
  if (temp < (setpoint - HYSTER_WARM))  return true;
  return false;
}

  // digitalWrite(RELAY_PIN_WARM, relayStateWarm );