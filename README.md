# RooDe
People counter working with MySensors
Repository initialized with version 0.9.4 and MySensors 2.3.0-alpha
## Featurelist
* Counting people entering and leaving a room
* Sensor threhsold calibration with standard deviation
* OLED support

## Hardware
* Arduino Pro Mini 5V 
* 2x sharp gp2y0a02yk0f 20-150cm
* 1x HC-SR501
* 1x NRF24L01+ incl. Power Adapter
  * Alternative: RFM69 or ESP8266
* 1x CP2101 Micro USB TTL Adapter
* 1x 128x32 OLED Display (Optional)
* 2x Push-Buttons (Optional)
* Case (see .stl files) -> coming soon

## Changelog
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
