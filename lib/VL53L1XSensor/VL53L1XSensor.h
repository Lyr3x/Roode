#ifndef VL53L1XSensor_H
#define VL53L1XSensor_H

#include <Config.h>
#include "Arduino.h"
#include "../vl53l1_api/vl53l1_api.h"
#include <Wire.h>

class VL53L1XSensor
{
public:
  VL53L1XSensor(int XSHUT, int I2C_ADDRESS);
  ~VL53L1XSensor(){};
  void init();
  int readRangeContinuoisMillimeters(VL53L1_UserRoi_t roiConfig);
  void setROI(int leftRoiConfig[4], int rightRoiConfig[4]);
  void setRangeMode(int mode);
  void setPresetMode(int mode);
  void startMeasurement();
  void stopMeasurement();
  void timeoutOccured();
  uint16_t getThreshold();
  void checkDev(VL53L1_DEV Dev);
  int threshold;
  
private:
  VL53L1_Dev_t sensor;
  VL53L1_DEV Sensor = &sensor;
  int _XSHUT;
  int _I2C_ADDRESS;
  int _INT;
  VL53L1_UserRoi_t leftRoiConfig = {10, 15, 15, 0}; //TopLeftX, TopLeftY, BotRightX, BotRightY
  VL53L1_UserRoi_t rightRoiConfig = {0, 15, 5, 0};  //TopLeftX, TopLeftY, BotRightX, BotRightY
  VL53L1_UserRoi_t roiConfig;
  int status;
  static VL53L1_RangingMeasurementData_t RangingData;
};

#endif