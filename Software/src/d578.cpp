#include <Arduino.h>

#include "d578.h"

unsigned long lastKeepAlive;

boolean keepAlive(void)
{
    if ((millis() - lastKeepAlive) >= 1000)
    {
        lastKeepAlive = millis();
        return 1;
    }
    else
    {
        return 0;
    }
}