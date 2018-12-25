#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* RooDe Configuration file
The predefined config enables most of the features and uses the NRF24L01+ Radio module
Be carfeul with reconfiguring! Some options shouldnt be changed!
*/

/*
  RooDe version number
  Please update frequently
### Changelog v0.9.6:
* added full VL53L0X support
  * measruing speed improvements 
  * Fixed LONG_RANGE mode which gave just -1 as result
  * Fixed receiving and sending message issues
  * Added OLED brightness config option
  * general bug fixes and improvements
*/
#define ROODE_VERSION "0.9.6"

/* 
###### MySensors Configuration ######
DESCRIPTION
*/

// #define MY_DEBUG                       //!!Comment out in production mode!! Its not possible to test all features of roode wiht DEBUG mode actiavted due to performance issues.
#define MY_RADIO_RF24                 //Define for using NRF24L01+ radio
#define MY_RF24_PA_LEVEL RF24_PA_HIGH //Options are: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH or RF24_PA_MAX. MAX will use more power but will transmit the furthest
// #define MY_RADIO_RFM69                 // Define for using RFM69 radio
// #define MY_RFM69_FREQUENCY RF69_433MHZ // Define for frequency setting. Needed if you're radio module isn't 868Mhz (868Mhz is default in lib)
// #define MY_IS_RFM69HW                  // Mandatory if you radio module is the high power version (RFM69HW and RFM69HCW), Comment it if it's not the case
#define MY_NODE_ID 10 // Set a static Node ID if needed

/* #### Security Functions ####
If the MySensors Gateway uses the Signing feature you can enable this here too.
When using the SIMPLE_PASSWD Flag the SIGNING_SOFT flag is set implicit.
Signing should only be used without MY_DEBUG on 328p's due to memory problems. 
Otherwise we cant ensure that the system will run smooth and without any problems. 
The usage WEAK_SECURITY is not advised but maybe the only solution besides a dedicated ATSHA204A module.
*/

// #define MY_SIGNING_SOFT
// #define MY_SIGNING_SIMPLE_PASSWD "a68PHafobmV8jeXN"
// #define MY_DEBUG_VERBOSE_SIGNING
// #define MY_SIGNING_WEAK_SECURITY

/*
###### Feature selection ######
  * USE_OLED for OLED 128x32 support
  * USE_BATTERY when powering the controller with an Lithium battery
  * CALIBRATION for calibrating the Sensors on startup
  * USE_COUNTER_BUTTONS
  * USE_ENERGY_SAVING If enabled the sensor will stop measuring when no motion is detected. 
                      We still want to process incoming messages and request updated values from the controller,
                      hence we nee to perform this after every measuring loop
*/
// #define USE_SHARP_IR
#define USE_VL53L0X
// #define USE_VL53L1X
#define USE_OLED // Activates OLED 128x32 support including brightness control.
// #define USE_BATTERY (preconfigured for Lithium-Ion (4.2V))
#define USE_MOTION
#define CALIBRATION //enables calibration of the distance sensors and motion sensor initializing
// #define USE_COUNTER_BUTTONS
#define USE_ENEGERY_SAVING

/* 
###### IR Sensor setup ######
*/
#define LTIME 10000 // loop time (should not be lower than 8 seconds)
#define MTIME 800   // measuring/person
#if defined(USE_SHARP_IR) && !defined(USE_VL53L0X)
#define CALIBRATION_VAL 4000 //read X values (X/2 from each sensor) and calculate the max value
#define ANALOG_IR_SENSORR 0  //IR Room Analog Pin
#define ANALOG_IR_SENSORC 2  //IR Corridor Analog Pin
#define ROOM_ENABLE 7        //IR Sensor Digital Pin for Room - EN Pin
#define CORRIDOR_ENABLE 8    //IR Sensor Digital Pin for Corridor - EN Pin
#define THRESHOLD_X 200      // x is the value added to the calibrated value
//#define IR_BOOT 30 // Not needed for the new sensors caused by the enable pin
#endif

#if defined(USE_VL53L0X) && !defined(USE_SHARP_IR)
#include <VL53L0X.h>
#include <Wire.h>
#define CORRIDOR_SENSOR_newAddress 42
#define ROOM_SENSOR_newAddress 43
#define ROOM_XSHUT 7     //XSHUT Pin
#define CORRIDOR_XSHUT 8 //XSHUT Pin

#define CALIBRATION_VAL 500 //read X values (X/2 from each sensor) and calculate the max value
#define THRESHOLD_X 300     // x is the value added to the calibrated value

/*
 Feature switches:
 * If possible use HIGH_SPEED mode, which works in a range withing 1.2m fine
 * If you got en error code just toggle off HIGH_SPEED to off.
 * If you are still receiving an unreliable reading/error code turn on LONG_RANGE mode which
   is working for up to 2m with the VL53L0X or 4m with the VL53L1X.
*/
#define HIGH_SPEED // 1.2m accuracy +- 5%
// #define LONG_RANGE //supports ranged up to 2m
// #define HIGH_ACCURACY // 1.2m accuracy < +-3%

#endif

#if defined(USE_VL53L1X) && !defined(USE_SHARP_IR)
#include <VL53L1X.h>
#include <Wire.h>
#define CORRIDOR_SENSOR_newAddress 41
#define ROOM_SENSOR_newAddress 42
#define ROOM_ENABLE 7     //XSHUT Pin
#define CORRIDOR_ENABLE 8 //XSHUT Pin
static VL53L1X CORRIDOR_SENSOR;
static VL53L1X ROOM_SENSOR;
#define THRESHOLD_X 300 // x is the value added to the calibrated value
#define LONG_RANGE

#endif

/* OLED setup 
  For now only the OLED 128x32 monochrom displays are supported without modification
  For the bigger 128x64 OLED's the SSD1306_text.h must be modified
*/
#ifdef USE_OLED
// includes for OLED 128x32 and 128x64 support
/* use minimal lib */
#include <SSD1306_text.h>
static SSD1306_text oled;
#define BRIGHTNESS_CTRL 0x81 // Do not change this value. This starts the Brightness control mode
#define BRIGHTNESS 1         //Set the OLED brightness value to a val between 1 and 255
#endif

/* 
###### Motion Sensor setup ###### 
*/
#ifdef USE_MOTION
#include <MotionSensor.h>
#define DIGITAL_INPUT_SENSOR 2 // motion sensor digital pin (2 or 3 because just those pins are interrupt pins)
#ifdef MY_DEBUG
#define MOTION_INIT_TIME 1
#else
#define MOTION_INIT_TIME 60 //initialization time in seconds
#endif
/* Motion Sensor setup*/
static MotionSensor motion(DIGITAL_INPUT_SENSOR);
#endif

/*
###### Push-Button Setup ######
*/
#ifdef USE_COUNTER_BUTTONS
#define DECREASE_BUTTON 5
#define INCREASE_BUTTON 6
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
#endif //Include guard