#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/i2c/i2c.h"
#include <EEPROM.h>
#include <VL53L1X.h>
#include <math.h>

namespace esphome
{
  namespace roode
  {
#define NOBODY 0
#define SOMEONE 1
#define VERSION "v1.3.1"
#define EEPROM_SIZE 512
    static int LEFT = 0;
    static int RIGHT = 1;
    // MQTT Commands
    static int resetCounter = 0;
    static int forceSetValue = -1;

    /*
    ##### CALIBRATION ##### 
    */
    static int center[2] = {0, 0}; /* center of the two zones */
    static int zone = 0;

    /*
    Use the VL53L1X_SetTimingBudget function to set the TB in milliseconds. The TB values available are [15, 20,
    33, 50, 100, 200, 500]. This function must be called after VL53L1X_SetDistanceMode.
    Note: 15 ms only works with Short distance mode. 100 ms is the default value.
    The TB can be adjusted to improve the standard deviation (SD) of the measurement. 
    Increasing the TB, decreases the SD but increases the power consumption.
    */

    static int delay_between_measurements = 0;
    static int time_budget_in_ms = 0;

    // parameters which define the time between two different measurements in various modes (https://www.st.com/resource/en/datasheet/vl53l1x.pdf)
    static int time_budget_in_ms_short = 15;      // 20ms with the full API but 15ms with the ULD API (https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf)
    static int time_budget_in_ms_long = 33;       // Works up to 3.1m increase to 140ms for 4m
    static int time_budget_in_ms_max_range = 200; // Works up to 4m in the dark on a white chart
    static int delay_between_measurements_short = 25;
    static int delay_between_measurements_long = 50;
    static int delay_between_measurements_max = 220;

    class Roode : public PollingComponent
    {
    public:
      void setup() override;
      void update() override;
      void loop() override;
      void dump_config() override;

      void set_calibration(bool val) { calibration_ = val; }
      void set_roi_calibration(bool val) { roi_calibration_ = val; }
      void set_max_threshold_percentage(int val) { max_threshold_percentage_ = val; }
      void set_min_threshold_percentage(int val) { min_threshold_percentage_ = val; }
      void set_roi_height(int height) { roi_height_ = height; }
      void set_roi_width(int width) { roi_width_ = width; }
      void set_address(uint64_t address) { this->address_ = address; }
      void set_invert_direction(bool dir) { invert_direction_ = dir; }
      void set_restore_values(bool val) { restore_values_ = val; }
      void set_advised_sensor_orientation(bool val) { advised_sensor_orientation_ = val; }
      void set_distance_sensor(sensor::Sensor *distance_sensor_) { distance_sensor = distance_sensor_; }
      void set_people_counter_sensor(sensor::Sensor *people_counter_sensor_) { people_counter_sensor = people_counter_sensor_; }
      void set_max_threshold_zone0_sensor(sensor::Sensor *max_threshold_zone0_sensor_) { max_threshold_zone0_sensor = max_threshold_zone0_sensor_; }
      void set_max_threshold_zone1_sensor(sensor::Sensor *max_threshold_zone1_sensor_) { max_threshold_zone1_sensor = max_threshold_zone1_sensor_; }
      void set_min_threshold_zone0_sensor(sensor::Sensor *min_threshold_zone0_sensor_) { min_threshold_zone0_sensor = min_threshold_zone0_sensor_; }
      void set_min_threshold_zone1_sensor(sensor::Sensor *min_threshold_zone1_sensor_) { min_threshold_zone1_sensor = min_threshold_zone1_sensor_; }
      void set_roi_height_sensor(sensor::Sensor *roi_height_sensor_) { roi_height_sensor = roi_height_sensor_; }
      void set_roi_width_sensor(sensor::Sensor *roi_width_sensor_) { roi_width_sensor = roi_width_sensor_; }
      void set_presence_sensor_binary_sensor(binary_sensor::BinarySensor *presence_sensor_) { presence_sensor = presence_sensor_; }
      void set_version_text_sensor(text_sensor::TextSensor *version_sensor_) { version_sensor = version_sensor_; }
      void set_entry_exit_event_text_sensor(text_sensor::TextSensor *entry_exit_event_sensor_) { entry_exit_event_sensor = entry_exit_event_sensor_; }
      void set_i2c_parent(i2c::I2CComponent *parent);
      void set_i2c_address(uint8_t address);
      void getZoneDistance();
      void sendCounter(uint16_t counter);
      void recalibration();

      uint16_t distance = 0;
      int DIST_THRESHOLD_MAX[2] = {0, 0}; // max treshold of the two zones
      int DIST_THRESHOLD_MIN[2] = {0, 0}; // min treshold of the two zones
      int roi_width_{6};                  // width of the ROI
      int roi_height_{16};                // height of the ROI
      uint64_t peopleCounter{0};

    protected:
      VL53L1X distanceSensor;
      sensor::Sensor *distance_sensor = new sensor::Sensor();
      sensor::Sensor *people_counter_sensor = new sensor::Sensor();
      sensor::Sensor *max_threshold_zone0_sensor = new sensor::Sensor();
      sensor::Sensor *max_threshold_zone1_sensor = new sensor::Sensor();
      sensor::Sensor *min_threshold_zone0_sensor = new sensor::Sensor();
      sensor::Sensor *min_threshold_zone1_sensor = new sensor::Sensor();
      sensor::Sensor *roi_height_sensor = new sensor::Sensor();
      sensor::Sensor *roi_width_sensor = new sensor::Sensor();
      binary_sensor::BinarySensor *presence_sensor = new binary_sensor::BinarySensor();
      text_sensor::TextSensor *version_sensor = new text_sensor::TextSensor();
      text_sensor::TextSensor *entry_exit_event_sensor = new text_sensor::TextSensor();

      void roi_calibration(VL53L1X distanceSensor);
      void calibration(VL53L1X distanceSensor);
      void setCorrectDistanceSettings(float average_zone_0, float average_zone_1);

      bool calibration_{true};
      bool roi_calibration_{false};
      bool advised_sensor_orientation_{true};
      uint64_t address_ = 0;
      bool invert_direction_{false};
      bool restore_values_{false};
      uint64_t max_threshold_percentage_{85};
      uint64_t min_threshold_percentage_{0};
      float sum_zone_0 = 0;
      float sum_zone_1 = 0;
      int number_attempts = 20;
      float average_zone_0 = 0;
      float average_zone_1 = 0;
      int left = 0, right = 0, oldcnt;
      boolean lastTrippedState = 0;
      double people, distance_avg;
      int short_distance_threshold = 1300;
      int long_distance_threshold = 3100;
      bool status = false;
    };

  } // namespace roode
} // namespace esphome
