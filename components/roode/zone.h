#pragma once
#include <math.h>

#include "VL53L1X_ULD.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/optional.h"
#include "orientation.h"
#include "configuration.h"

static const char *const TAG = "Zone";
static const char *const CALIBRATION = "Zone calibration";
namespace esphome {
namespace roode {
struct ROI {
  uint8_t width;
  uint8_t height;
  uint8_t center;
  void set_width(uint8_t width) { this->width = width; }
  void set_height(uint8_t height) { this->height = height; }
  void set_center(uint8_t center) { this->center = center; }
};

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

class Zone {
 public:
  explicit Zone(uint8_t id) : id{id} {};
  VL53L1_Error readDistance(VL53L1X_ULD &distanceSensor);
  void calibrateThreshold(VL53L1X_ULD &distanceSensor, int number_attempts);
  void roi_calibration(VL53L1X_ULD &distanceSensor, uint16_t entry_threshold, uint16_t exit_threshold,
                       Orientation orientation);
  const uint8_t id;
  uint16_t getDistance() const;
  uint16_t getMinDistance() const;
  ROI *roi = new ROI();
  Threshold *threshold = new Threshold();

 protected:
  int getOptimizedValues(int *values, int sum, int size);
  uint16_t distance;
  int *Distances;
  uint8_t sample_size;
  uint8_t samples;
  uint16_t min_distance;
};
}  // namespace roode
}  // namespace esphome
