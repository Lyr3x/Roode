# RooDe
People counter working with MySensors
Repository initialized with version 0.9.4 and MySensors 2.3.0-alpha
## Featurelist
* Counting people entering and leaving a room
* Sensor threhsold calibration with standard deviation
* Receiving commands from the controller e.g. recalibration or manual counter setting
* OLED support
* Domoticz Events provided (Roode is useable with every MySensors Controller)

## Hardware
* Arduino Pro Mini 5V 
* 2x VL53L0X/VL53L1X or ~~SHARP gp2y0a02yk0f 20-150cm.~~(dont buy them anymore!)
    * The VL53XXX are using I²C and are much more reliable up to 2m
    * The Sharp Sensors are analog sensors
* 1x HC-SR501 
    * For waking up RooDe. No unnecessary measurements if nobodoy is in range.
* 1x NRF24L01+ incl. Power Adapter
  * Alternative: RFM69 or ESP8266 (not implemented yet)
* 1x CP2101 Micro USB TTL Adapter
    * Power Supply and Programming Interface
* 1x 128x32 OLED Display (Optional)
* 2x Push-Buttons (Optional)
* Case (see .stl files

## Additional Configuration Info
Everything the user needs to do is walking through the Configuration.h and setup all the features which should be used. There you can also see which pins are pre defined for all the devices and you can easily change them to different free pins. 
It is crucial that you turned off `#define MY_DEBUG`! Otherwise Roode will crash while performing certain tasks like receiving messages due to high frequent serial output. The Debug mode is just for developing purposes. 

## Changelog
### Changelog v0.9.6
#### Additions
* added full VL53L0X support
* Added OLED brightness config option
* Updated to MySensors@2.3.1
#### Bugfixes and other changes
* measruing speed improvements 
* Fixed receiving and sending message issues
* General bug fixes and improvements
* Changed default PA_LEVEL to HIGH

### Changelog v0.9.5
* Added VL53L0X support
    * Using the Pololu VL53XXX Library
* Huge Code refactor

### Changelog v0.9.4-alpha2:
* added incomplete push button support
    * There is a need of 2 spare interrupt PINS
    * May be working with PinChangeInterrupt library
* minor bugfixes
* rename send() function and changed signature
### Changelog v0.9.4-beta:
* changed IR-calibration hardcoded value to THRESHOLD_X 
* CALIBRATION_VAL to 4000
### Changelog v.0.9.4-release
* Added standard deviation threhsold calculation
* Removed constant THRESHOLD_X
