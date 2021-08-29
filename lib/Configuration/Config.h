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
#define CALIBRATIONV2

/**
###### VL53L1X Definition ######
**/
#ifdef USE_VL53L1X
#include <VL53L1X.h>
#endif //USE_VL53L1X
#ifdef USE_VL53L1X

#endif //CONFIG_H