#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <VL53L1XSensor.h>
#include <Config.h>
#include <Math.h>
int calculateStandardDeviation(int irValues[]);
void calibration(VL53L1XSensor Sensor);
#endif