#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <SendCounter.h>
#include "core/MySensorsCore.h"
#include <Math.h>
#include <Configuration.h> 
#include <SensorReader.h>

int calibration();
int calculateStandardDeviation(int irValues[]);

#endif