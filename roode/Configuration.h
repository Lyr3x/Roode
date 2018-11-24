/* RooDe Configuration file
The predefined config enables most of the features and uses the NRF24L01+ Radio module
Be carfeul with reconfiguring! 
*/

/*
  RooDe version number
  Please update frequently
  Changelog v0.9.4-alpha2:
  - added incomplete push button support
    - There is a need of 2 spare interrupt PINS
    - May be working with PinChangeInterrupt library
  - minor bugfixes
  - rename send() function and changed signature
  Changelog v0.9.4-beta:
  - changed IR-calibration hardcoded value to THRESHOLD_X 
  - CALIBRATION_VAL to 4000
*/
#define ROODE_VERSION "0.9.4"

/* 
###### MySensors Configuration ######
DESCRIPTION
*/

// #define MY_DEBUG                       //Comment out in production mode
#define MY_RADIO_NRF24                 //Define for using NRF24L01+ radio
// #define MY_RADIO_RFM69                 // Define for using RFM69 radio
// #define MY_RFM69_FREQUENCY RF69_433MHZ // Define for frequency setting. Needed if you're radio module isn't 868Mhz (868Mhz is default in lib)
// #define MY_IS_RFM69HW                  // Mandatory if you radio module is the high power version (RFM69HW and RFM69HCW), Comment it if it's not the case
// #define MY_NODE_ID 1                   // Set a static Node ID if needed


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
  * CALIBRATION for calibrating the IR Sensors on startup
  * USE_COUNTER_BUTTONS
*/
#define USE_OLED
// #define USE_BATTERY (preconfigured for Lithium-Ion (4.2V))
#define CALIBRATION //enables calibration of the irsensors and motion sensor initializing
#define USE_COUNTER_BUTTONS

/* 
###### IR Sensor setup ######
*/
#define ANALOG_IR_SENSORR 0  //IR Room Analog Pin
#define ANALOG_IR_SENSORC 2  //IR Corridor Analog Pin
#define IR_D_R 7             //IR Sensor Digital Pin for Room - EN Pin
#define IR_D_C 8             //IR Sensor Digital Pin for Corridor - EN Pin
#define LTIME 10000          // loop time (should not be lower than 8 seconds)
#define MTIME 800            // measuring/person
#define CALIBRATION_VAL 4000 //read X values (X/2 from each sensor) and calculate the max value
#define THRESHOLD_X 300  // x is the value added to the calibrated value
//#define IR_BOOT 30 // Not needed for the new sensors caused by the enable pin

/* OLED setup 
  For now only the OLED 128x32 monochrom displays are supported without modification
  For the bigger 128x64 OLED's the SSD1306_text.h must be modified
*/
#ifdef USE_OLED
// includes for OLED 128x32 and 128x64 support
/* use minimal lib */
#include <SSD1306_text.h>
SSD1306_text oled;
#endif

/* 
###### Motion Sensor setup ###### 
*/
#define DIGITAL_INPUT_SENSOR 2 // motion sensor digital pin (2 or 3 because just those pins are interrupt pins)
unsigned int MOTION_INIT_TIME = 10; //initialization time in seconds

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
#define CHILD_ID_BATTERY 2                          //MySensors Battery child ID
#define BATTERY_FULL 4.2                            // a 18650 lithium ion battery usually give 4.2V when full
#define BATTERY_ZERO 3.5                            // 2.4V limit for 328p at 16MHz. 1.9V, limit for nrf24l01 without
#endif