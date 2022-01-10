
#pragma once
#include <math.h>

#include "VL53L1X_ULD.h"
#include "configuration.h"
#include "esphome/core/log.h"

static const char *const TAG = "Zone";
static const char *const CALIBRATION = "Zone calibration";
namespace esphome {
namespace roode {
struct ROI {
  uint16_t width;
  uint16_t height;
  uint16_t center;
};
struct Threshold {
  uint16_t min;
  uint16_t min_percentage;
  uint16_t max;
  uint16_t max_percentage;
};
class Zone {
 public:
  Zone(int roi_width, int roi_height, int roi_center, int sample_size);
  VL53L1_Error readDistance(VL53L1X_ULD &distanceSensor);
  int calibrateThreshold(VL53L1X_ULD &distanceSensor, int number_attempts, uint16_t max_threshold_percentage,
                         uint16_t min_threshold_percentage);
  void roi_calibration(VL53L1X_ULD &distanceSensor, int entry_threshold, int exit_threshold, bool sensor_orientation);
  uint16_t calibrateRoi();
  uint16_t getMinThreshold();
  uint16_t getMaxThreshold();
  uint16_t getRoiWidth();
  uint16_t getRoiHeight();
  uint16_t getRoiCenter();
  void setMinThreshold(uint16_t min);
  void setMaxThreshold(uint16_t max);
  void setRoiWidth(uint16_t new_roi_width);
  void setRoiHeight(uint16_t new_roi_height);
  void setRoiCenter(uint16_t new_roi_center);
  void updateRoi(uint16_t new_width, uint16_t new_height);
  uint8_t getZoneId();
  uint16_t getDistance();
  uint16_t getMinDistance();
  bool handleSensorStatus();

 protected:
  int getSum(int *values, int size);
  int getOptimizedValues(int *values, int sum, int size);
  ROI roi;
  Threshold threshold;
  uint16_t roi_width;
  uint16_t roi_height;
  uint16_t roi_center;
  uint8_t id;
  uint16_t distance;
  int *Distances;
  uint8_t sample_size;
  uint8_t samples;
  uint16_t min_distance;
};
}  // namespace roode
}  // namespace esphome
