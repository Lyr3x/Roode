#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "EEPROM.h"
#include <VL53L1X.h>
#include <math.h>

namespace esphome
{
  namespace roode
  {
#define NOBODY 0
#define SOMEONE 1

    static const char *TAG = "main";
    static int LEFT = 0;
    static int RIGHT = 1;
    // MQTT Commands
    static int resetCounter = 0;
    static int forceSetValue = -1;

    /*
    ##### CALIBRATION ##### 
    */
    static int MIN_DISTANCE[] = {0, 0};
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

    // this value has to be true if the sensor is oriented as in Duthdeffy's picture
    static bool advised_orientation_of_the_sensor = true;

    // this value has to be true if you don't need to compute the threshold every time the device is turned on
    static bool save_calibration_result = true;

    // parameters which define the time between two different measurements in longRange mode
    static int delay_between_measurements_long = 50;
    static int time_budget_in_ms_long = 33; // Works up to 3.1m increase to 140ms for 4m
    static int delay_between_measurements_short = 25;
    static int time_budget_in_ms_short = 15;

    class Roode : public PollingComponent
    {
    public:
      Roode() : PollingComponent(5000) {}
      sensor::Sensor *distance_sensor = new sensor::Sensor();
      sensor::Sensor *people_counter_sensor = new sensor::Sensor();
      sensor::Sensor *threshold_zone0_sensor = new sensor::Sensor();
      sensor::Sensor *threshold_zone1_sensor = new sensor::Sensor();
      sensor::Sensor *roi_height_sensor = new sensor::Sensor();
      sensor::Sensor *roi_width_sensor = new sensor::Sensor();
      binary_sensor::BinarySensor *presence_sensor = new binary_sensor::BinarySensor();
      void dump_config() override;
      void setup() override;
      void update() override;

      void set_calibration(bool val) { calibration_ = val; }
      void set_threshold_percentage(int val) { threshold_percentage_ = val; }
      void set_roi_height(int height) { roi_height_ = height; }
      void set_roi_width(int width) { roi_width_ = width; }
      void set_address(uint64_t address) { this->address_ = address; }
      void set_invert_direction(bool dir) { invert_direction_ = dir; }
      void set_update_interval(uint32_t update_interval) { this->update_interval_ = update_interval; }
      void set_distance_sensor(sensor::Sensor *distance_sensor_) { distance_sensor = distance_sensor_; }
      void set_people_counter_sensor(sensor::Sensor *people_counter_sensor_) { people_counter_sensor = people_counter_sensor_; }
      void set_threshold_zone0_sensor(sensor::Sensor *threshold_zone0_sensor_) { threshold_zone0_sensor = threshold_zone0_sensor_; }
      void set_threshold_zone1_sensor(sensor::Sensor *threshold_zone1_sensor_) { threshold_zone1_sensor = threshold_zone1_sensor_; }
      void set_roi_height_sensor(sensor::Sensor *roi_height_sensor_) { roi_height_sensor = roi_height_sensor_; }
      void set_roi_width_sensor(sensor::Sensor *roi_width_sensor_) { roi_width_sensor = roi_width_sensor_; }
      void set_presence_sensor(binary_sensor::BinarySensor *presence_sensor_) { presence_sensor = presence_sensor_; }
      void checkMQTTCommands();

      void publishMQTT(int val);

      void getZoneDistance();
      void sendCounter();
      void loop() override;

      void roi_calibration(VL53L1X distanceSensor);
      void calibration(VL53L1X distanceSensor);

      void calibration_boot(VL53L1X distanceSensor);
      void set_i2c_parent(i2c::I2CComponent *parent);
      void set_i2c_address(uint8_t address);
      uint16_t distance = 0;
      int DIST_THRESHOLD_MAX[2] = {0, 0}; // treshold of the two zones
      int roi_width_{6};
      int roi_height_{16};
      uint64_t peopleCounter{0};
      uint64_t recalibrate{0};

    protected:
      VL53L1X distanceSensor;
      bool calibration_{true};
      bool roi_calibration_{false};
      uint64_t address_ = 0;
      bool invert_direction_{true};
      uint64_t threshold_percentage_{85};
      uint64_t update_interval_;
      float sum_zone_0 = 0;
      float sum_zone_1 = 0;
      int number_attempts = 20;
      float average_zone_0 = 0;
      float average_zone_1 = 0;
      int left = 0, right = 0, oldcnt;
      boolean lastTrippedState = 0;
      //static int num_timeouts = 0;
      double people, distance_avg;
      // value which defines the threshold which activates the short distance mode (the sensor supports it only up to a distance of 1300 mm)
      int short_distance_threshold = 1300;
    };

  } // namespace roode
} // namespace esphome