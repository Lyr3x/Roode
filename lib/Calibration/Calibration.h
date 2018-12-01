#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <SendCounter.h>
#include "core/MySensorsCore.h"
#include <Math.h>
#include <Configuration.h>
#include <SensorReader.h>
#ifdef USE_VL53L0X
#include <VL53L0X.h>
#endif

int calibration();
int calculateStandardDeviation(int irValues[]);

#endif