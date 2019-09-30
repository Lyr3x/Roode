# RooDe
People counter working with any smart home sysme which supports MQTT
Repository initialized with version 0.9.4 and MySensors 2.3.0-alpha
**Version 1.0 will break the support for all hardware configurations working right now**
## Version 1.0 - Featurelist
Here is a list of features which are working or in _work_
* Counting people entering and leaving a room
* Sensor threhsold calibration with standard deviation
* Receiving commands from the controller e.g. recalibration or manual counter setting
* OLED support
* Sleep mode with Motion Sensor support
* Domoticz Events provided (Roode is useable with every MySensors Controller)
* _Introduction of the new Sensor - VL53L1X_
* _Completely configurable over WebUI_
* _Firmware upgrades over WebUI_
* _Automatically creates Domoticz devices_

## Hardware
* ESP8266 or ESP32
* 1x VL53L1X
* Optional HC-SR501
* Optional 128x32 OLED
* Power Supply
* Encolsure (see .stl files

## Changelog
### Changelog v1.0
#### Additions and Breaking Changes
* Add full VL53L1X support
* Removes legacy support which includes
   * Arduino plattform
   * MySensors plattform
   * **Non** VL53LXX-Sensors
* MQTT support
* Automatic device creation
* Configuration WebUI (may be moved to a later version)
   * Removal of Config.h
   
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
