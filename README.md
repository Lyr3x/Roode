# RooDe

[![GitHub release](https://img.shields.io/github/v/tag/Lyr3x/Roode?style=flat-square)](https://GitHub.com/Lyr3x/Roode/releases/)
[![Build](https://img.shields.io/github/workflow/status/Lyr3x/Roode/CI?style=flat-square)](https://github.com/Lyr3x/Roode/blob/master/.github/workflows/ci.yml)
[![Maintenance](https://img.shields.io/maintenance/yes/2023?style=flat-square)](https://GitHub.com/Lyr3x/Roode/graphs/commit-activity)

[![Roode community](https://img.shields.io/discord/879407995837087804.svg?label=Discord&logo=Discord&colorB=7289da&style=for-the-badge)](https://discord.gg/hU9SvSXMHs)

People counter working with any smart home system which supports ESPHome/MQTT like Home Assistant. All necessary entities are created automatically.

- [Hardware Recommendation](#hardware-recommendation)
- [Wiring](#wiring)
  - [ESP32](#esp32)
  - [ESP8266](#esp8266)
- [Configuration](#configuration)
  - [Platform Setup](#platform-setup)
  - [Sensors](#sensors)
  - [Threshold distance](#threshold-distance)
- [Algorithm](#algorithm)
- [FAQ/Troubleshoot](#faqtroubleshoot)

## Hardware Recommendation

- ESP8266 or ESP32
  - **Wemos D1 Mini ESP32** <-- Recommended
  - Wemos D1 mini (ESP8266)
  - NodeMCU V2
- 1x VL53L1X
  - **Pololu** <-- Recommended
  - GY-53
  - Black PCB chinese sensor
  - Pimoroni
- 1A Power Supply **Do not use an USB port of your computer!**
- Encolsure (see .stl files) - will be updated soon!
  Pins:
  SDA_PIN 4 (ESP8266) or 21 (ESP32)
  SCL_PIN 5 (ESP8266) or 22 (ESP32)

## Wiring

The sensors from Pololu, Adafruit and the GY-53 can also be connected to the 5v pin (VIN) as they have an voltage regulator.

If you use a GY-53 you need to connect GND the PS (Ps=0) pin.

Ps=1 (default): Serial port UART mode, Pin3 is TX, Pin4 is RX, TTL level, PWM output works.
Ps=0 (when connected to GND): In the IIC mode, the user can operate the chip by himself. The module owns the MCU and does not operate the chip. The PWM output does not work.

### ESP32

```
                    ESP32   VL53L1X board
-------------------------   -------------
                      3V3 - VIN
                      GND - GND
     SDA (pin 42, GPIO21) - SDA
     SCL (pin 39, GPIO22) - SCL
```

### ESP8266

```
                  ESP8266   VL53L1X board
-------------------------   -------------
                      3V3 - VIN
                      GND - GND
              D2 (GPIO 4) - SDA
              D1 (GPIO 5) - SCL
```

## Configuration

## Platform Setup

Roode is provided as an external_component which means it is easy to setup in any ESPHome sensor configuration file.

Other than base ESPHome configuration the only config that's needed for Roode is

```yaml
external_components:
  - source: github://Lyr3x/Roode@master
    refresh: always
vl53l1x:
roode:
```

This uses the recommended default configuration.

However, we offer a lot of flexibility. Here's the full configuration spelled out.

```yml
external_components:
  - source: github://Lyr3x/Roode
    refresh: always
    ref: master

# VL53L1X sensor configuration is separate from Roode people counting algorithm
vl53l1x:
  # A non-standard I2C address
  address:

  # Sensor calibration options
  calibration:
    # The ranging mode is different based on how long the distance is that the sensor need to measure.
    # The longer the distance, the more time the sensor needs to take a measurement.
    # Available options are: auto, shortest, short, medium, long, longer, longest
    ranging: auto
    # The offset correction distance. See calibration section (WIP) for more details.
    offset: 8mm
    # The corrected photon count in counts per second. See calibration section (WIP) for more details.
    crosstalk: 53406cps

  # Hardware pins
  pins:
    # Shutdown/Enable pin, which is needed to change the I2C address. Required with multiple sensors.
    xshut: GPIO3
    # Interrupt pin. Use to notify us when a measurement is ready. This feature is WIP.
    # This needs to be an internal pin.
    interrupt: GPIO1

# Roode people counting algorithm
roode:
  # Smooth out measurements by using the minimum distance from this number of readings
  sampling: 2

  # The orientation of the two sensor pads in relation to the entryway being tracked.
  # The advised orientation is parallel, but if needed this can be changed to perpendicular.
  orientation: parallel

  # This controls the size of the Region of Interest the sensor should take readings in.
  # The current default is
  roi: { height: 16, width: 6 }
  # We have an experiential automatic mode that can be enabled with
  # roi: auto
  # or only automatic for one dimension
  # roi: { height: 16, width: auto }

  # The detection thresholds for determining whether a measurement should count as a person crossing.
  # A reading must be greater than the minimum and less than the maximum to count as a crossing.
  # These can be given as absolute distances or as percentages.
  # Percentages are based on the automatically determined idle or resting distance.
  detection_thresholds:
    min: 0% # default minimum is any distance
    max: 85% # default maximum is 85%
    # an example of absolute units
    # min: 50mm
    # max: 234cm

  # The people counting algorithm works by splitting the sensor's capability reading area into two zones.
  # This allows for detecting whether a crossing is an entry or exit based on which zones was crossed first.
  zones:
    # Flip the entry/exit zones. If Roode seems to be counting backwards, set this to true.
    invert: false

    # Entry/Exit zones can set overrides for individual ROI & detection thresholds here.
    # If omitted, they use the options configured above.
    entry:
      # Entry zone will automatically configure ROI, regardless of ROI above.
      roi: auto
    exit:
      roi:
        # Exit zone will have a height of 8 and a width of number set above or default or auto
        height: 8
        # Additionally, zones can manually set their center point.
        # Usually though, this is left for Roode to automatically determine.
        center: 124

      detection_thresholds:
        # Exit zone's min detection threshold will be 5% of idle/resting distance, regardless of setting above.
        min: 5%
        # Exit zone's max detection threshold will be 70% of idle/resting distance, regardless of setting above.
        max: 70%
```

Also feel free to check out running examples for:
- [Wemos D1 mini with ESP32](peopleCounter32.yaml)
- [Wemos D1 mini with ESP8266](peopleCounter8266.yaml)

### Sensors

#### People Counter

The most important one is the people counter.

```yaml
number:
  - platform: roode
    people_counter:
      name: People Count
```

Regardless of how close we can get, people counting will never be perfect.
This allows the current people count to be adjusted easily via Home Assistant.

#### Other sensors available

```yaml
binary_sensor:
  - platform: roode
    presence:
      name: $friendly_name Presence
    zones:
      entry:
        presence:
          name: $friendly_name Entry Presence
      exit:
        presence:
          name: $friendly_name Exit Presence

sensor:
  - platform: roode
    id: hallway
    distance_sensor:
      name: $friendly_name distance
      filters:
        - delta: 100.0
    threshold_entry:
      name: $friendly_name Zone 0
    threshold_exit:
      name: $friendly_name Zone 1
    roi_height:
      name: $friendly_name ROI height
    roi_width:
      name: $friendly_name ROI width

text_sensor:
  - platform: roode
    version:
      name: $friendly_name version
  - platform: roode
    entry_exit_event:
      name: $friendly_name last direction
```

### Threshold distance

Another crucial choice is the one corresponding to the threshold. Indeed a movement is detected whenever the distance read by the sensor is below this value. The code contains a vector as threshold, as one (as myself) might need a different threshold for each zone.

The threshold is automatically calculated by the sensor. To do so it is necessary to position the sensor and, after turning it on, wait for 10 seconds without passing under it. After this time, the average of the measures for each zone will be computed and the thereshold for each ROI will correspond to 80% of the average value. Also the value of 80% can be modified in the code, by editing the variable `max_threshold_percentage` and `min_threshold_percentage`.

If you install the sensor e.g 20cm over a door you dont want to count the door open and closing. In this case you should set the `min_threshold_percentage` to about `10`.

Example:

```
Mounting height:    2200mm
Door height:        2000mm
Person height:      1800mm
max_threshold_percentage: 80% = 1760
min_threshold_percentage: 10% = 200

All distances smaller then 200mm and greater then 1760mm will be ignored.
```

## Algorithm

The implemented Algorithm is an improved version of my own implementation which checks the direction of a movement through two defined zones. ST implemented a nice and efficient way to track the path from one to the other direction. I migrated the algorigthm with some changes into the Roode project.
The concept of path tracking is the detecion of a human:

- In the first zone only
- In both zones
- In the second zone only
- In no zone

That way we can ensure the direction of movement.

The sensor creates a 16x16 grid and the final distance is computed by taking the average of the distance of the values of the grid.
We are defining two different Region of Interest (ROI) inside this grid. Then the sensor will measure the two distances in the two zones and will detect any presence and tracks the path to receive the direction.

However, the algorithm is very sensitive to the slightest modification of the ROI, regarding both its size and its positioning inside the grid.

ST Microelectronics define the values for the parameters as default like this:

The center of the ROI you set is based on the table below and the optical center has to be set as the pad above and to the right of your exact center:

Set the center SPAD of the region of interest (ROI)
based on VL53L1X_SetROICenter() from STSW-IMG009 Ultra Lite Driver

ST user manual UM2555 explains ROI selection in detail, so we recommend
reading that document carefully. Here is a table of SPAD locations from
UM2555 (199 is the default/center):

```
128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248
129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249
130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250
131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251
132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252
133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253
134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254
135,143,151,159,167,175,183,191,  199,207,215,223,231,239,247,255

127,119,111,103, 95, 87, 79, 71,   63, 55, 47, 39, 31, 23, 15,  7
126,118,110,102, 94, 86, 78, 70,   62, 54, 46, 38, 30, 22, 14,  6
125,117,109,101, 93, 85, 77, 69,   61, 53, 45, 37, 29, 21, 13,  5
124,116,108,100, 92, 84, 76, 68,   60, 52, 44, 36, 28, 20, 12,  4
123,115,107, 99, 91, 83, 75, 67,   59, 51, 43, 35, 27, 19, 11,  3
122,114,106, 98, 90, 82, 74, 66,   58, 50, 42, 34, 26, 18, 10,  2
121,113,105, 97, 89, 81, 73, 65,   57, 49, 41, 33, 25, 17,  9,  1
120,112,104, 96, 88, 80, 72, 64,   56, 48, 40, 32, 24, 16,  8,  0 <- Pin 1
```

This table is oriented as if looking into the front of the sensor (or top of
the chip). SPAD 0 is closest to pin 1 of the VL53L1X, which is the corner
closest to the VDD pin on the Pololu VL53L1X carrier board:

```
  +--------------+
  |             O| GPIO1
  |              |
  |             O|
  | 128    248   |
  |+----------+ O|
  ||+--+  +--+|  |
  |||  |  |  || O|
  ||+--+  +--+|  |
  |+----------+ O|
  | 120      0   |
  |             O|
  |              |
  |             O| VDD
  +--------------+
```

However, note that the lens inside the VL53L1X inverts the image it sees
(like the way a camera works). So for example, to shift the sensor's FOV to
sense objects toward the upper left, you should pick a center SPAD in the
lower right.

## FAQ/Troubleshoot

**Question:** Why is the Sensor not measuring the correct distances?

**Answer:** This can happen in various scenarios. I try to list causes sorted by likelyhood

1. You did not remove the protection film (most times its yellow)
2. You did not connect the Sensor properly
3. Light interference (You will see a lot of noise)
4. Bad connections

## Sponsors

Thank you very much for you sponsorship!

- sunshine-hass
