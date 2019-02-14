#ifndef VL53L0XSensor_H
#define VL53L0XSensor_H

#include "Arduino.h"
#include <VL53L0X.h>
#include <Sensor.h>
#include <Calibration.h>
#include <Config.h>
#include <Wire.h>

class VL53L0XSensor : public Sensor
{
public:
  VL53L0XSensor(int XSHUT, int I2C_ADDRESS);
  ~VL53L0XSensor(){};
  void init();
  uint16_t readData();
  int calibration();
  void setMode(int mode);
  void startContinuous(uint32_t period_ms = 0);
  void stopContinuous();
  uint16_t readRangeContinuousMillimeters(void);
  uint16_t readRangeSingleMillimeters(void);
  void setTimeout(uint16_t timeout);
  uint16_t getTimeout(void);
  bool timeoutOccurred(void);
  uint16_t getThreshold();
  

private:
int threshold;
  VL53L0X _Sensor;
  int _XSHUT;
  int _I2C_ADDRESS;
};

#endif