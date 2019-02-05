#ifndef MYSENSORS_TRANSMITTER_H
#define MYSENSORS_TRANSMITTER_H
#include <Transmitter.h>
#include "core/MySensorsCore.h"
// #include <MySensors.h>
//override delay
__attribute__((weak)) void delay(unsigned long ms)
{
    wait(ms);
}

class MySensorsTransmitter : public Transmitter
{
  public:
    struct Devices
    {
        MyMessage threshold;
        MyMessage peoplecounter;
        MyMessage info;
        MyMessage room_sensor_value;
        MyMessage corridor_sensor_value;
        MyMessage room_switch;
    };
    MySensorsTransmitter::MySensorsTransmitter();
    Devices devices;
    int transmit(MyMessage &message, int val = 0, const char *text = "");
    void presentation();
    int receive(const MyMessage &message);
};
#endif
