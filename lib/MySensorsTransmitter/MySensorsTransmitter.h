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
// static MyMessage msg(CHILD_ID_R, V_STATUS);    //room on/off child
// static MyMessage pcMsg(CHILD_ID_PC, V_TEXT);   //people counter child
// static MyMessage thrMsg(CHILD_ID_THR, V_TEXT); //Threshold and recalibration child
// MyMessage *test = new MyMessage msg2(CHILD_ID_PC, V_STATUS);

class MySensorsTransmitter : Transmitter
{
  public:
    struct Devices
    {
        // MyMessage msg(CHILD_ID_R, V_STATUS);
        MyMessage threshold;
        MyMessage peoplecounter;
        MyMessage info;
        MyMessage room_sensor_value;
        MyMessage corridor_sensor_value;
        MyMessage room_switch;
    };
    MySensorsTransmitter::MySensorsTransmitter();
    Devices devices;
    int transmit(MyMessage &message, int val=0, String text="");
    void presentation();
    int receive(const MyMessage &message);
};
#endif
