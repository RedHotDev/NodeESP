

#pragma once
#include <Arduino.h>

#define Fan_PWM_FRQ 25000



class Cooler {
public:

    

    // тикер - вызывать в прерывании по фронту
    void tick() {
        if (ticks <= window) {
            if (!ticks) tmr = micros();
            if (ticks == window) tmr = micros() - tmr;
            ticks++;
        }
    }
    
    // установка количества тиков для счёта времени (по умолч 10)
    void setWindow(uint8_t nwindow) {
        window = nwindow;
        ticks = 0;
    }
    
    // таймаут прерываний (мс), после которого считается что вращение прекратилось (по умолч 1000)
    void setTimeout(uint16_t ntout) {
        tout = ntout;
    }
    
    // получить обороты в минуту
    uint32_t getRPM() {
        return getTime() ? (60000000ul / prd) : 0;
    }


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
    
    uint8_t window = 10;
    uint32_t tmrMs = millis();
    uint32_t prd = 0;
    uint16_t tout = 1000;
};
