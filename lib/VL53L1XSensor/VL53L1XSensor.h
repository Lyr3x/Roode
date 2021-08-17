#ifndef VL53L1XSensor_H
#define VL53L1XSensor_H
#pragma once

#include <vl53l1_api.h>
#include <Config.h>
#include <Wire.h>

class VL53L1XSensor
{
public:
  VL53L1XSensor(int XSHUT, int I2C_ADDRESS);
  ~VL53L1XSensor(){};
  void init();
  int readRangeContinuoisMillimeters(VL53L1_UserRoi_t roiConfig,int delay_ms=0);
  void setRangeMode(int mode);
  void setPresetMode(int mode);
  void startMeasurement();
  void stopMeasurement();
  void timeoutOccured();
  uint16_t getThreshold();
  void setThreshold(uint16_t newThreshold);
  void checkDev();
  
private:
  VL53L1_RangingMeasurementData_t RangingData;
  VL53L1_Dev_t sensor;
  VL53L1_DEV Sensor = &sensor;
  int _XSHUT;
  int _I2C_ADDRESS;
  int _INT;
  VL53L1_UserRoi_t roiConfig;
  int status;
};

#endif