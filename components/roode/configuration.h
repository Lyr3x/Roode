
#pragma once
#include <math.h>

#include "VL53L1X_ULD.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "zone.h"

static VL53L1_Error last_sensor_status = VL53L1_ERROR_NONE;
static VL53L1_Error sensor_status = VL53L1_ERROR_NONE;
namespace esphome {
namespace roode {
class Configuration {
 public:
  void setSensorMode(VL53L1X_ULD distanceSensor, int sensor_mode, int timing_budget = 0);
  int getTimingBudget();

 protected:
  VL53L1X_ULD distanceSensor;
  VL53L1_Error sensor_status = VL53L1_ERROR_NONE;
  void setSensorMode(int sensor_mode, int timing_budget = 0);
  void publishSensorConfiguration(int DIST_THRESHOLD_ARR[2], bool isMax);
  void setCorrectDistanceSettings(float average_entry_zone_distance, float average_exit_zone_distance);
};
}  // namespace roode
}  // namespace esphome
