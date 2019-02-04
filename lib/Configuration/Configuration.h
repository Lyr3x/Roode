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
#define ROODE_VERSION "0.9.6-beta"

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
#define USE_VL53L0X
// #define USE_VL53L1X
// #define USE_OLED // Activates OLED 128x32 support including brightness control.
// #define USE_BATTERY (preconfigured for Lithium-Ion (4.2V))
#define USE_MOTION
#define CALIBRATION //enables calibration of the distance sensors and motion sensor initializing
#define USE_ENEGERY_SAVING
#define USE_MQTT // If one is using an ESP8266 uncomment this to use MQTT
// #define USE_MYSENSORS // If one is using an Arduino with NRF24L01+ uncomment this to use MySensors

#ifdef USE_MQTT
// Setup MQTT IDX for Domoticz
#define ROOM_MQTT "256"
#define CORRIDOR_MQTT "257"
#define HOME_SWITCH "258"
#define INFO "259"
#define PEOPLECOUNTER "260"
#elif USE_MYSENSORS
// MySensors ID Setup
#define CHILD_ID_ROOM_SWITCH 0
#define CHILD_ID_PEOPE_COUNTER 1
#define CHILD_ID_THRESHOLD 3
#define CHILD_ID_INFO 4
#endif
/* 
###### IR Sensor setup ######
*/
#define LTIME 10000 // loop time (should not be lower than 8 seconds)
#define MTIME 800   // measuring/person
#ifdef USE_VL53L0X
#include <VL53L0X.h>
#include <Wire.h>
#define CORRIDOR_SENSOR_newAddress 42
#define ROOM_SENSOR_newAddress 43
#ifdef USE_MQTT
#define ROOM_XSHUT D3     //XSHUT Pin
#define CORRIDOR_XSHUT D4 //XSHUT Pin
#endif

#ifdef USE_MYSENSORS
#define ROOM_XSHUT 7     //XSHUT Pin
#define CORRIDOR_XSHUT 8 //XSHUT Pin

#endif //USE_MYSENSORS

// I2C PIN definition
#define SDA_PIN D6
#define SCL_PIN D5

#define CALIBRATION_VAL 500 //read X values (X from each sensor) and calculate the max value and standard deviation
#define THRESHOLD_X 300     // x is the value added to the calibrated value

/*
 Feature switches:
 * If possible use HIGH_SPEED mode, which works in a range withing 1.2m fine
 * If you got en error code just toggle off HIGH_SPEED to off.
 * If you are still receiving an unreliable reading/error code turn on LONG_RANGE mode which
   is working for up to 2m with the VL53L0X or 4m with the VL53L1X.
*/
#define HIGH_SPEED 0    // 1.2m accuracy +- 5%
#define LONG_RANGE 1    //supports ranged up to 2m
#define HIGH_ACCURACY 2 // 1.2m accuracy < +-3%
#define MODE = HIGH_SPEED;

#endif

// #if defined(USE_VL53L1X) && !defined(USE_SHARP_IR)
// #include <VL53L1X.h>
// #include <Wire.h>
// #include <VL53L1XWrap.h>
// #define CORRIDOR_SENSOR_newAddress 41
// #define ROOM_SENSOR_newAddress 42
// #define ROOM_XSHUT 7     //XSHUT Pin
// #define CORRIDOR_XSHUT 8 //XSHUT Pin

// #define CALIBRATION_VAL 500 //read X values (X/2 from each sensor) and calculate the max value
// #define THRESHOLD_X 300     // x is the value added to the calibrated value
// #define LONG_RANGE

// #endif

/* OLED setup 
  For now only the OLED 128x32 monochrom displays are supported without modification
  For the bigger 128x64 OLED's the SSD1306_text.h must be modified
*/
#ifdef USE_OLED
// includes for OLED 128x32 and 128x64 support
/* use minimal lib */
#ifdef(USE_MYSENSORS)
#include <SSD1306_text.h>
static SSD1306_text oled;
#endif

#ifdef(USE_MQTT)
#include <SSD1306Wire.h>
static SSD1306Wire oled(0x3c, SDA_PIN, SCL_PIN);
#endif

#define BRIGHTNESS_CTRL 0x81 // Do not change this value. This starts the Brightness control mode
#define BRIGHTNESS 1         //Set the OLED brightness value to a val between 1 and 255
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

#endif // #endif //Include guard