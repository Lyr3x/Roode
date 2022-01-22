#pragma once
#include <math.h>

#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/optional.h"
#include "../vl53l1x/vl53l1x.h"
#include "orientation.h"

using TofSensor = esphome::vl53l1x::VL53L1X;
using esphome::vl53l1x::ROI;

static const char *const TAG = "Zone";
static const char *const CALIBRATION = "Zone calibration";
namespace esphome {
namespace roode {
struct Threshold {
  /** Automatically determined idling distance (average of several measurements) */
  uint16_t idle;
  uint16_t min;
  optional<uint8_t> min_percentage{};
  uint16_t max;
  optional<uint8_t> max_percentage{};
  void set_min(uint16_t min) { this->min = min; }
  void set_min_percentage(uint8_t min) { this->min_percentage = min; }
  void set_max(uint16_t max) { this->max = max; }
  void set_max_percentage(uint8_t max) { this->max_percentage = max; }
};

class Zone : public PollingComponent {
 public:
  explicit Zone(uint8_t id) : PollingComponent(), id{id} {};
  void dump_config() override;
  void update() override;
  VL53L1_Error readDistance(TofSensor *distanceSensor);
  void reset_roi(uint8_t default_center);
  void calibrateThreshold(TofSensor *distanceSensor, int number_attempts);
  void roi_calibration(uint16_t entry_threshold, uint16_t exit_threshold, Orientation orientation);
  const uint8_t id;
  bool is_occupied() const { return occupancy->state; };
  ROI *roi = new ROI();
  ROI *roi_override = new ROI();
  Threshold *threshold = new Threshold();
  void set_max_samples(uint8_t max) { max_samples = max; };
  binary_sensor::BinarySensor *occupancy = new binary_sensor::BinarySensor();
  void set_occupancy_sensor(binary_sensor::BinarySensor *sensor) { occupancy = sensor; }
  void set_distance_sensor(sensor::Sensor *sensor) { distance_sensor = sensor; }

 protected:
  int getOptimizedValues(int *values, int sum, int size);
  VL53L1_Error last_sensor_status = VL53L1_ERROR_NONE;
  VL53L1_Error sensor_status = VL53L1_ERROR_NONE;
  uint16_t last_distance;
  uint16_t min_distance;
  std::vector<uint16_t> samples;
  uint8_t max_samples;
  sensor::Sensor *distance_sensor{nullptr};
};
}  // namespace roode
}  // namespace esphome
