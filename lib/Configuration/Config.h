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

/*
  RooDe version number
  Please update frequently
### Changelog v1.0:
* Complete architecture refactor
  * New abstraction layers:
    * Sensor
    * Transmitter
  * Easy to extend with the type of Sensors and Transmitters
* Support for ESP8266 and MQTT
* Support for Arduino with MySensors
* Removed SHARP_IR infrared Sensors
* Changed Message types
* Added new message type INFO 
* Independent sensor calibration
* added full VL53L0X support
  * measruing speed improvements 
  * Fixed LONG_RANGE mode which gave just -1 as result
  * Fixed receiving and sending message issues
  * Added OLED brightness config option
  * general bug fixes and improvements
* added untested VL53L1X support
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
#define XSHUT_PIN 14
/**
###### VL53L1X Definition ######
**/
#ifdef USE_VL53L1X
#include <VL53L1X.h>
#define SENSOR_I2C 0x52

//#define INT			D7 not used right now
#endif //USE_VL53L1X

#ifdef CALIBRATION
#define LTIME 10000         // loop time - should not be lower than 7s. Recommended is 10s
#define MTIME 800           // measuring/person (after 800ms a mis measure of one sensor is cleared)
#define CALIBRATION_VAL 200 //read X values (X from each sensor) and calculate the max value and standard deviation
#define DIST_THRESHOLD_MAX 1780
#endif
/*
 Feature switches:
 * If possible use HIGH_SPEED mode, which works in a range withing 1.2m fine
 * If you got en error code just toggle off HIGH_SPEED to off.
 * If you are still receiving an unreliable reading/error code turn on LONG_RANGE mode which
   is working for up to  2m with the VL53L0X or 4m with the VL53L1X.
*/
#ifdef USE_VL53L1X

enum SensorRangeModes
{
  SHORT_RANGE = 0,
  MEDIUM_RANGE = 1,
  LONG_RANGE = 2,
};

enum SensorPresetModes
{
  LITE_RANING = 0,
  AUTONOMOUS = 1,
  LOW_POWER_AUTONOMOUS = 2
};

#define SENSOR_RANGE_MODE MEDIUM_RANGE
#define SENSOR_PRESET_MODE AUTONOMOUS
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