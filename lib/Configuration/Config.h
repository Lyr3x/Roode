/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/
#ifndef CONFIG_H
#define CONFIG_H
#include <Wire.h>
#include <Arduino.h>
/* RooDe Configuration file
The predefined config enables most of the features and uses the NRF24L01+ Radio module
Be carfeul with reconfiguring! Some options shouldnt be changed!
*/

#define ROODE_VERSION "1.1-beta"
/*
###### FEATURE SELECTION ######
*/
#define USE_VL53L1X
// #define USE_OLED // Activates OLED 128x32 support including brightness control.
// #define USE_BATTERY (preconfigured for Lithium-Ion (4.2V))
// #define CALIBRATION //enables calibration of the distance sensors and motion sensor initializing
#define CALIBRATIONV2
/*
###### I2C Pin Definition ######
*/
#define SDA_PIN D2
#define SCL_PIN D1
/**
###### VL53L1X Definition ######
**/
#ifdef USE_VL53L1X

#define SENSOR_I2C 0x52
#include "SparkFun_VL53L1X.h"
SFEVL53L1X countSensor(Wire);
static int DIST_THRESHOLD_MAX[] = {0, 0}; // treshold of the two zones
static int MIN_DISTANCE[] = {0, 0};
static int center[2] = {0, 0}; /* center of the two zones */
static int Zone = 0;
static int ROI_height = 0;
static int ROI_width = 0;
// #define INVERT_DIRECTION //this will invert the direction of the sensor
//#define INT			D7 not used right now
#endif //USE_VL53L1X

#ifdef CALIBRATION
#define LTIME 10000         // loop time - should not be lower than 7s. Recommended is 10s
#define MTIME 800           // measuring/person (after 800ms a mis measure of one sensor is cleared)
#define CALIBRATION_VAL 200 //read X values (X from each sensor) and calculate the max value and standard deviation
#define DIST_THRESHOLD_MAX 1780
#endif

/* 
###### OLED Definition ###### 
  For now only the OLED 128x32 monochrom displays are supported without modification
  For the bigger 128x64 OLED's the SSD1306_text.h must be modified
*/

// #ifdef USE_OLED
// #include <Wire.h>
// #include "SSD1306Ascii.h"
// #include "SSD1306AsciiWire.h"

// #define OLED_I2C 0x3c
// // Define proper RST_PIN if required.
// #define RST_PIN -1
// #define BRIGHTNESS 1 //Set the OLED brightness value to a val between 0 and 255
// static SSD1306AsciiWire oled;
// #endif

/*
###### Battery Module ######
Keep in Mind that you need an Voltage regulator to stable 5V!
*/
#ifdef USE_BATTERY
#define BATTERY_METER_PIN 3
#define CHILD_ID_BATTERY 2 //MySensors Battery child ID
#define BATTERY_FULL 4.2   // a 18650 lithium ion battery usually give 4.2V when full
#define BATTERY_ZERO 3.5   // 2.4V limit for 328p at 16MHz. 1.9V, limit for nrf24l01 without
#endif
#endif //CONFIG_H