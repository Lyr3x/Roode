#ifndef SENSORREADER_H
#define SENSORREADER_H
// #include <Configuration.h>
#include <SendCounter.h>
#include "core/MySensorsCore.h"
void readSensorData(VL53L0X ROOM_SENSOR, VL53L0X CORRIDOR_SENSOR);
// some needed var declarations
extern int irrVal;    //analog value store for the room sensor
extern int ircVal;    //analog value store for the corridor sensor
extern int threshold; //if CALIBRATION is not defined, this threshold is used (okay for a 80cm doorway using reflection foil or white paper)

#endif