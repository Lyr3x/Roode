/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/
#ifndef CONFIG_H
#define CONFIG_H

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
#define ROODE_VERSION "1.0-alpha"
// #define MY_DEBUG                       //!!Comment out in production mode!! Its not possible to test all features of roode wiht DEBUG mode actiavted due to performance issues.
/*
###### FEATURE SELECTION ######
*/
#define USE_VL53L0X
// #define USE_VL53L1X
#define USE_OLED // Activates OLED 128x32 support including brightness control.
// #define USE_BATTERY (preconfigured for Lithium-Ion (4.2V))
#define USE_MOTION
#define CALIBRATION        //enables calibration of the distance sensors and motion sensor initializing
#define USE_ENEGERY_SAVING //v1.0-alpha note: needs more testing

/*
###### MySensors configuration ######
*/
#ifdef USE_MYSENSORS
#define MY_RADIO_RF24                 //Define for using NRF24L01+ radio
#define MY_RF24_PA_LEVEL RF24_PA_HIGH //Options are: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH or RF24_PA_MAX. MAX will use more power but will transmit the furthest
// #define MY_RADIO_RFM69                 // Define for using RFM69 radio
// #define MY_RFM69_FREQUENCY RF69_433MHZ // Define for frequency setting. Needed if you're radio module isn't 868Mhz (868Mhz is default in lib)
// #define MY_IS_RFM69HW                  // Mandatory if you radio module is the high power version (RFM69HW and RFM69HCW), Comment it if it's not the case
#define MY_NODE_ID 100 // Set a static Node ID if needed
#endif

#ifdef USE_MQTT
// Setup MQTT IDX for Domoticz
#define ROOM_MQTT "256"
#define CORRIDOR_MQTT "257"
#define ROOM_SWITCH_MQTT "258"
#define INFO_MQTT "259"
#define PEOPLECOUNTER_MQTT "260"
#define THRESHOLD_MQTT "261"
#endif
#ifdef USE_MYSENSORS
enum MysensorsId
{
  CHILD_ID_ROOM_SWITCH = 0,
  CHILD_ID_PEOPLECOUNTER = 1,
  CHILD_ID_THRESHOLD = 3,
  CHILD_ID_INFO = 4
};
#endif

/*
###### I2C Pin Definition ######
*/
#define SDA_PIN D6
#define SCL_PIN D5

/*
###### VL53L0X/VL53L1X Definition ######
*/
#ifdef USE_VL53L0X
#include <VL53L0X.h>
#include <Wire.h>
#endif //USE_VL53L0X

/**
 * Dont actiavte the VL53L1X Sensor yet.
 * Because of the single sensor setup there is a huge amount
 * of logic change necessary
 **/
#ifdef USE_VL53L1X
// #include <VL53L1XSensor.h>
#endif //USE_VL53L1X

#define CORRIDOR_SENSOR_newAddress 42
#define ROOM_SENSOR_newAddress 43
#ifdef USE_MQTT
#define ROOM_XSHUT D3     //XSHUT Pin
#define CORRIDOR_XSHUT D4 //XSHUT Pin
#endif                    //USE_MYSENSORS

#ifdef USE_MYSENSORS
#define ROOM_XSHUT 7     //XSHUT Pin
#define CORRIDOR_XSHUT 8 //XSHUT Pin

#endif                      //USE_MYSENSORS
#define LTIME 10000         // loop time - should not be lower than 7s. Recommended is 10s
#define MTIME 800           // measuring/person (after 800ms a mis measure of one sensor is cleared)
#define CALIBRATION_VAL 200 //read X values (X from each sensor) and calculate the max value and standard deviation

/*
 Feature switches:
 * If possible use HIGH_SPEED mode, which works in a range withing 1.2m fine
 * If you got en error code just toggle off HIGH_SPEED to off.
 * If you are still receiving an unreliable reading/error code turn on LONG_RANGE mode which
   is working for up to 2m with the VL53L0X or 4m with the VL53L1X.
*/
#ifdef USE_VL53L0X
enum SensorModes
{
  HIGH_SPEED = 0,   // 1.2m accuracy +- 5%
  LONG_RANGE = 1,   //supports ranged up to 2m
  HIGH_ACCURACY = 2 // 1.2m accuracy < +-3%
};
#define SENSOR_MODE HIGH_SPEED
#elif defined USE_VL53L1X

enum SensorRangeModes
{
  SHORT_RANGE = 0,
  MEDIUM_RANGE = 1,
  LONG_RANGE = 2,
};

enum SensorPresetModes˝
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

#ifdef USE_OLED
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define OLED_I2C 0x3c
// Define proper RST_PIN if required.
#define RST_PIN -1
#define BRIGHTNESS 1 //Set the OLED brightness value to a val between 0 and 255
static SSD1306AsciiWire oled;
#endif

/* 
###### Motion Sensor setup ###### 
*/
#ifdef USE_MOTION
#include <MotionSensor.h>

#ifdef USE_MYSENSORS
#define DIGITAL_INPUT_SENSOR 2 // motion sensor digital pin (2 or 3 because just those pins are interrupt pins)
#endif

#ifdef USE_MQTT
#define DIGITAL_INPUT_SENSOR D2 // motion sensor digital pin (2 or 3 because just those pins are interrupt pins)
#endif

#ifdef MY_DEBUG
#define MOTION_INIT_TIME 1
#else
#define MOTION_INIT_TIME 1 //initialization time in seconds
#endif

#ifndef DIGITAL_INPUT_SENSOR
#define DIGITAL_INPUT_SENSOR 2
#endif
/* Motion Sensor setup*/
static MotionSensor motion(DIGITAL_INPUT_SENSOR);
#endif

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