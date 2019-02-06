extern int BATTERY_SENSE_PIN;
extern int oldBatteryPcnt;
extern long oldvoltage;
extern int sensorValue;
extern int batteryPcnt;
extern float voltage;

#ifndef BatteryMeter_h
#define BatteryMeter_h

#include "core/MySensorsCore.h"
#include "SPI.h" 
#include "Arduino.h"
class BatteryMeter
{
  public:
    BatteryMeter(int pin);
    float checkBatteryLevel();
  private:
    int voltagePin;
};

#endif