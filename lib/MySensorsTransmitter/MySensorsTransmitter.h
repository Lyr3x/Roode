#ifndef MySensorsTransmitter_H
#define MySensorsTransmitter_H
#include "core/MySensorsCore.h"
//override delay
#ifndef delay
#define delay _delay_milliseconds
#endif
void _delay_milliseconds(unsigned int millis)
{
    wait(millis);
}
#endif