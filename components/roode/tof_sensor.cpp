#include "tof_sensor.h"

namespace esphome {
namespace roode {

void TofSensor::dump_config() {
  ESP_LOGCONFIG("VL53L1X", "dump config:");
  if (this->address.has_value()) {
    ESP_LOGCONFIG("VL53L1X", "  Address: 0x%02X", this->address.value());
  }
}

void TofSensor::setup() {
  Wire.begin();
  Wire.setClock(400000);

  // TODO use xshut_pin, if given, to change address
  auto status = this->address.has_value() ? this->sensor.Begin(this->address.value()) : this->sensor.Begin();
  if (status != VL53L1_ERROR_NONE) {
    // If the sensor could not be initialized print out the error code. -7 is timeout
    ESP_LOGE("VL53L1X", "Could not initialize the sensor, error code: %d", status);
    this->mark_failed();
    return;
  }

  if (this->offset.has_value()) {
    ESP_LOGI("VL53L1X", "Setting sensor offset calibration to %d", this->offset.value());
    status = this->sensor.SetOffsetInMm(this->offset.value());
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE("VL53L1X", "Could not set sensor offset calibration, error code: %d", status);
      this->mark_failed();
      return;
    }
  }

  if (this->xtalk.has_value()) {
    ESP_LOGI("VL53L1X", "Setting sensor xtalk calibration to %d", this->xtalk.value());
    status = this->sensor.SetXTalk(this->xtalk.value());
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE("VL53L1X", "Could not set sensor offset calibration, error code: %d", status);
      this->mark_failed();
      return;
    }
  }
}

void TofSensor::set_ranging_mode(const RangingMode *mode) {
  auto status = this->sensor.SetDistanceMode(mode->mode);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE("VL53L1X", "Could not set distance mode.  mode: %d", mode->mode);
  }

  status = this->sensor.SetTimingBudgetInMs(mode->timing_budget);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE("VL53L1X", "Could not set timing budget.  timing_budget: %d ms", mode->timing_budget);
  }

  status = this->sensor.SetInterMeasurementInMs(mode->delay_between_measurements);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE("VL53L1X", "Could not set measurement delay.  %d ms", mode->delay_between_measurements);
  }

  ESP_LOGI("VL53L1X", "Set ranging mode. timing_budget: %d, delay: %d, distance_mode: %d", mode->timing_budget,
           mode->delay_between_measurements, mode->mode);
}

optional<uint16_t> TofSensor::read_distance(ROI *roi, VL53L1_Error &status) {
  if (this->is_failed()) {
    return {};
  }

  status = this->sensor.SetROI(roi->width, roi->height);
  status += this->sensor.SetROICenter(roi->center);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGD("VL53L1X", "Could not set ROI, error code: %d", status);
    return {};
  }

  status = this->sensor.StartRanging();

  // Wait for the measurement to be ready
  // TODO use interrupt_pin, if given, to await data ready instead of polling
  uint8_t dataReady = false;
  while (!dataReady) {
    status += this->sensor.CheckForDataReady(&dataReady);
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGD("VL53L1X", "Data not ready yet, error code: %d", status);
      return {};
    }
    delay(1);
  }

  // Get the results
  uint16_t distance;
  status += this->sensor.GetDistanceInMm(&distance);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGD("VL53L1X", "Could not get distance, error code: %d", status);
    return {};
  }

  // After reading the results reset the interrupt to be able to take another measurement
  status = this->sensor.ClearInterrupt();
  status += this->sensor.StopRanging();
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGD("VL53L1X", "Could not stop ranging, error code: %d", status);
    return {};
  }

  return optional<uint16_t>(distance);
}

}  // namespace roode
}  // namespace esphome
