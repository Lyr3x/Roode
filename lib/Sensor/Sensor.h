#ifndef SENSOR_H
#define SENSOR_H

#include "Arduino.h"

class Sensor
{
public:
  virtual ~Sensor() {}
  virtual void init() = 0;
  virtual int readData() = 0;
  virtual int calibration() = 0;
};
#endif

