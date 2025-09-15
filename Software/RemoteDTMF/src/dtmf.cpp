#include <Arduino.h>

#include "dtmf.h"

volatile bool STQTick = 0;

void initDTMF(void)
{
    pinMode(STQ, INPUT);
    attachInterrupt(digitalPinToInterrupt(STQ), STQ_ISR, FALLING);
    pinMode(Q0, INPUT);
    pinMode(Q1, INPUT);
    pinMode(Q2, INPUT);
    pinMode(Q3, INPUT);
}

char readDTMF(void)
{
    uint8_t valRead = 0;
    if(STQTick == 1)
    {
        delayMicroseconds(50);
        valRead = 0 | (digitalRead(Q3) << 3) | (digitalRead(Q2) << 2) | (digitalRead(Q1) << 1) | (digitalRead(Q0));
        STQTick = 0;
        return charDTMF[valRead];
    }
    else
    {
        return 0;
    }
}

void STQ_ISR(void)
{
    STQTick = 1;
}