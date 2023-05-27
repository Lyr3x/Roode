#include "zone.h"

namespace esphome {
namespace roode {

void Zone::dump_config() {
  ESP_LOGCONFIG(TAG, "%s Zone:", id == 0U ? "Entry" : "Exit");
  ESP_LOGCONFIG(TAG, "  ROI: { width: %d, height: %d, center: %d }", roi->width, roi->height, roi->center);
  ESP_LOGCONFIG(TAG, "  Threshold: { min: %dmm (%d%%), max: %dmm (%d%%), idle: %dmm }", threshold->min,
                threshold->min_percentage.value_or((threshold->min * 100) / threshold->idle), threshold->max,
                threshold->max_percentage.value_or((threshold->max * 100) / threshold->idle), threshold->idle);
  LOG_UPDATE_INTERVAL(this);
}

VL53L1_Error Zone::readDistance(TofSensor *distanceSensor) {
  VL53L1_Error status;
  auto result = distanceSensor->read_distance(roi, status);
  if (!result.has_value()) {
    return status;
  }

  last_distance = result.value();
  samples.insert(samples.begin(), result.value());
  if (samples.size() > max_samples) {
    samples.pop_back();
  };
  min_distance = *std::min_element(samples.begin(), samples.end());

  occupancy->publish_state(min_distance < threshold->max && min_distance > threshold->min);

  return status;
  this->update_threshold(min_distance);
  return sensor_status;
}

void Zone::update_threshold(uint16_t distance) {
  if (distance > threshold->max) {
    idle_distances.insert(idle_distances.begin(), distance);
  }
  if (idle_distances.size() == 100) {
    this->threshold->idle = this->get_avg(idle_distances);
    if (this->threshold->max_percentage.has_value()) {
      this->threshold->max = (this->threshold->idle * this->threshold->max_percentage.value()) / 100;
    }
    if (this->threshold->min_percentage.has_value()) {
      this->threshold->min = (this->threshold->idle * this->threshold->min_percentage.value()) / 100;
    }
    idle_distances.clear();
  }
}

/**
 * This sets the ROI for the zone to the given overrides or the standard default.
 * This is needed to do initial calibration of thresholds & ROI.
 */
void Zone::reset_roi(uint8_t default_center) {
  roi->width = roi_override->width ?: 6;
  roi->height = roi_override->height ?: 16;
  roi->center = roi_override->center ?: default_center;
  ESP_LOGD(TAG, "%s ROI reset: { width: %d, height: %d, center: %d }", id == 0U ? "Entry" : "Exit", roi->width,
           roi->height, roi->center);
}

void Zone::calibrate_threshold(TofSensor *distanceSensor, int number_attempts) {
  ESP_LOGD(CALIBRATION, "Beginning. zoneId: %d", id);
  int *zone_distances = new int[number_attempts];
  int sum = 0;
  for (int i = 0; i < number_attempts; i++) {
    this->readDistance(distanceSensor);
    zone_distances[i] = last_distance;
    sum += zone_distances[i];
  };
  threshold->idle = this->get_optimized_values(zone_distances, sum, number_attempts);

  if (threshold->max_percentage.has_value()) {
    threshold->max = (threshold->idle * threshold->max_percentage.value()) / 100;
  }
  if (threshold->min_percentage.has_value()) {
    threshold->min = (threshold->idle * threshold->min_percentage.value()) / 100;
  }
  ESP_LOGI(CALIBRATION, "Calibrated threshold for zone. zoneId: %d, idle: %d, min: %d (%d%%), max: %d (%d%%)", id,
           threshold->idle, threshold->min,
           threshold->min_percentage.value_or((threshold->min * 100) / threshold->idle), threshold->max,
           threshold->max_percentage.value_or((threshold->max * 100) / threshold->idle));
}

void Zone::roi_calibration(uint16_t entry_threshold, uint16_t exit_threshold, Orientation orientation) {
  // the value of the average distance is used for computing the optimal size of the ROI and consequently also the
  // center of the two zones
  int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(entry_threshold, exit_threshold) / 1000)));
  int ROI_size = min(8, max(4, function_of_the_distance));
  this->roi->width = this->roi_override->width ?: ROI_size;
  this->roi->height = this->roi_override->height ?: ROI_size * 2;
  if (this->roi_override->center) {
    this->roi->center = this->roi_override->center;
  } else {
    // now we set the position of the center of the two zones
    if (orientation == Parallel) {
      switch (this->roi->width) {
        case 4:
          this->roi->center = this->id == 0U ? 150 : 247;
          break;
        case 5:
        case 6:
          this->roi->center = this->id == 0U ? 159 : 239;
          break;
        case 7:
        case 8:
          this->roi->center = this->id == 0U ? 167 : 231;
          break;
      }
    } else {
      switch (this->roi->width) {
        case 4:
          this->roi->center = this->id == 0U ? 193 : 58;
          break;
        case 5:
        case 6:
          this->roi->center = this->id == 0U ? 194 : 59;
          break;
        case 7:
        case 8:
          this->roi->center = this->id == 0U ? 195 : 60;
          break;
      }
    }
  }
  ESP_LOGI(CALIBRATION, "Calibrated ROI for zone. zoneId: %d, width: %d, height: %d, center: %d", id, roi->width,
           roi->height, roi->center);
}

int Zone::get_optimized_values(int *values, int sum, int size) {
  int sum_squared = 0;
  int variance = 0;
  int sd = 0;
  int avg = sum / size;

  for (int i = 0; i < size; i++) {
    sum_squared = sum_squared + (values[i] * values[i]);
    App.feed_wdt();
  }
  variance = sum_squared / size - (avg * avg);
  sd = sqrt(variance);
  ESP_LOGD(CALIBRATION, "Zone AVG: %d", avg);
  ESP_LOGD(CALIBRATION, "Zone SD: %d", sd);
  return avg - sd;
}

void Zone::update() {
  if (distance_sensor != nullptr) {
    distance_sensor->publish_state(min_distance);
  }
}
int Zone::get_avg(std::vector<uint16_t> values) {
  auto sum = std::accumulate(values.begin(), values.end(), 0);
  int avg = sum / values.size();
  return avg;
}
}  // namespace roode
}  // namespace esphome
