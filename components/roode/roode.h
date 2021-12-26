#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/application.h"
#include "EEPROM.h"
#include <VL53L1X.h>
#include <math.h>

namespace esphome
{
  namespace roode
  {
#define NOBODY 0
#define SOMEONE 1
#define VERSION "v1.4.0-beta"
#define EEPROM_SIZE 512
    static int LEFT = 0;
    static int RIGHT = 1;
    static const uint16_t DISTANCES_ARRAY_SIZE = 2;

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

    /*
    Parameters which define the time between two different measurements in various modes (https://www.st.com/resource/en/datasheet/vl53l1x.pdf)
    The timing budget and inter-measurement period should not be called when the sensor is
    ranging. The user has to stop the ranging, change these parameters, and restart ranging
    The minimum inter-measurement period must be longer than the timing budget + 4 ms.
    */
    static int time_budget_in_ms_short = 20;  // 20ms with the full API but 15ms with the ULD API (https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf)
    static int time_budget_in_ms_medium = 60; // Works up to 3.1m increase to minimum of 140ms for 4m
    static int time_budget_in_ms_long = 140;  // Works up to 4m in the dark on a white chart

    class Roode : public PollingComponent
    {
    public:
      void setup() override;
      void update() override;
      void loop() override;
      void dump_config() override;

      void set_calibration_active(bool val) { calibration_active_ = val; }
      void set_manual_active(bool val) { manual_active_ = val; }
      void set_roi_calibration(bool val) { roi_calibration_ = val; }
      void set_timing_budget(int timing_budget) { timing_budget_ = timing_budget; }
      void set_manual_threshold(int val) { manual_threshold_ = val; }
      void set_max_threshold_percentage(int val) { max_threshold_percentage_ = val; }
      void set_min_threshold_percentage(int val) { min_threshold_percentage_ = val; }
      void set_roi_height(int height) { roi_height_ = height; }
      void set_roi_width(int width) { roi_width_ = width; }
      void set_i2c_address(uint8_t address) { this->address_ = address; }
      void set_invert_direction(bool dir) { invert_direction_ = dir; }
      void set_restore_values(bool val) { restore_values_ = val; }
      void set_advised_sensor_orientation(bool val) { advised_sensor_orientation_ = val; }
      void set_use_sampling(bool val) { use_sampling_ = val; }
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
      void set_sensor_status_text_sensor(text_sensor::TextSensor *status_sensor_) { status_sensor = status_sensor_; }
      void set_sensor_mode(int sensor_mode_) { sensor_mode = sensor_mode_; }
      void getZoneDistance();
      void sendCounter(uint16_t counter);
      void recalibration();
      bool handleSensorStatus();

      uint16_t distance = 0;
      VL53L1X::RangeStatus last_sensor_status = VL53L1X::RangeStatus::None;
      VL53L1X::RangeStatus sensor_status = VL53L1X::RangeStatus::None;
      int DIST_THRESHOLD_MAX[2] = {0, 0}; // max treshold of the two zones
      int DIST_THRESHOLD_MIN[2] = {0, 0}; // min treshold of the two zones
      int roi_width_{6};                  // width of the ROI
      int roi_height_{16};                // height of the ROI
      uint16_t peopleCounter{0};

    protected:
      VL53L1X distanceSensor;
      sensor::Sensor *distance_sensor;
      sensor::Sensor *people_counter_sensor;
      sensor::Sensor *max_threshold_zone0_sensor;
      sensor::Sensor *max_threshold_zone1_sensor;
      sensor::Sensor *min_threshold_zone0_sensor;
      sensor::Sensor *min_threshold_zone1_sensor;
      sensor::Sensor *roi_height_sensor;
      sensor::Sensor *roi_width_sensor;
      binary_sensor::BinarySensor *presence_sensor;
      text_sensor::TextSensor *version_sensor;
      text_sensor::TextSensor *entry_exit_event_sensor;
      text_sensor::TextSensor *status_sensor;

      void roi_calibration(VL53L1X distanceSensor, int optimized_zone_0, int optimized_zone_1);
      void calibration(VL53L1X distanceSensor);
      void setCorrectDistanceSettings(float average_zone_0, float average_zone_1);
      void setSensorMode(int sensor_mode, int timing_budget = 0);
      void publishSensorConfiguration(int DIST_THRESHOLD_ARR[2], bool isMax);
      int getOptimizedValues(int *values, int sum, int size);
      int getSum(int *values, int size);
      bool calibration_active_{false};
      bool manual_active_{false};
      bool roi_calibration_{false};
      int sensor_mode{-1};
      bool advised_sensor_orientation_{true};
      bool use_sampling_{true};
      uint8_t address_ = 0x29;
      bool invert_direction_{false};
      bool restore_values_{false};
      uint64_t max_threshold_percentage_{85};
      uint64_t min_threshold_percentage_{0};
      uint64_t manual_threshold_{2000};
      int number_attempts = 20;
      int timing_budget_{-1};
      int short_distance_threshold = 1300;
      int medium_distance_threshold = 3100;
      bool status = false;
      int optimized_zone_0;
      int optimized_zone_1;
    };

  } // namespace roode
} // namespace esphome
