/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/
#ifndef CONFIG_H
#define CONFIG_H
#include <Wire.h>
#include <Arduino.h>

#define USE_VL53L1X
// #define USE_OLED // Activates OLED 128x32 support including brightness control.
#define CALIBRATIONV2

/**
###### VL53L1X Definition ######
**/
#ifdef USE_VL53L1X
#include <VL53L1X.h>
#endif //USE_VL53L1X
#ifdef USE_VL53L1X

#endif //CONFIG_H