#include "vl53l1x.h"

namespace esphome {
namespace vl53l1x {

void VL53L1X::dump_config() {
  ESP_LOGCONFIG(TAG, "VL53L1X:");
  LOG_I2C_DEVICE(this);
  if (this->ranging_mode != nullptr) {
    ESP_LOGCONFIG(TAG, "  Ranging: %s", this->ranging_mode->name);
  }
  if (offset.has_value()) {
    ESP_LOGCONFIG(TAG, "  Offset: %dmm", this->offset.value());
  }
  if (xtalk.has_value()) {
    ESP_LOGCONFIG(TAG, "  XTalk: %dcps", this->xtalk.value());
  }
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin.value());
  LOG_PIN("  XShut Pin: ", this->xshut_pin.value());
}

void VL53L1X::setup() {
  ESP_LOGD(TAG, "Beginning setup");

  // TODO use xshut_pin, if given, to change address
  auto status = this->init();
  if (status != VL53L1_ERROR_NONE) {
    this->mark_failed();
    return;
  }
  this->sensor.Init();
  ESP_LOGD(TAG, "Device initialized");

  if (this->offset.has_value()) {
    ESP_LOGI(TAG, "Setting offset calibration to %d", this->offset.value());
    status = this->sensor.SetOffsetInMm(this->offset.value());
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Could not set offset calibration, error code: %d", status);
      this->mark_failed();
      return;
    }
  }

  if (this->xtalk.has_value()) {
    ESP_LOGI(TAG, "Setting crosstalk calibration to %d", this->xtalk.value());
    status = this->sensor.SetXTalk(this->xtalk.value());
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Could not set crosstalk calibration, error code: %d", status);
      this->mark_failed();
      return;
    }
  }

  ESP_LOGI(TAG, "Setup complete");
}

VL53L1_Error VL53L1X::init() {
  ESP_LOGD(TAG, "Trying to initialize");

  VL53L1_Error status;
  status = wait_for_boot();
  if (status != VL53L1_ERROR_NONE) {
    return status;
  }

  // Check if the device has booted. If not a timeout has occured
  if (address_ != this->sensor.GetI2CAddress()) {
    ESP_LOGD(TAG, "Setting new I2C address. new: %#x old: %#x", address_, this->sensor.GetI2CAddress());
    // Begin was initialized with another I2C address. This could mean the init would fail as the sensor was already
    // initialized before. The I2C address only changes when calling SetI2CAddress or by being powered down.
    status = this->sensor.SetI2CAddress(address_);
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Could not set I2C address, error code: %d", status);
    }
    ESP_LOGD(TAG, "I2C address set. new: %#x", this->sensor.GetI2CAddress());
    delay(100);
    status = wait_for_boot();
    if (status != VL53L1_ERROR_NONE) {
      return status;
    }
    ESP_LOGD(TAG, "Found device, initializing...");
    status = this->sensor.Init();
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Could not initialize device, error code: %d", status);
      return status;
    }
    return status;
  }
}

VL53L1_Error VL53L1X::wait_for_boot() {
  // Wait for firmware to copy NVM device_state into registers
  delayMicroseconds(1200);

  uint8_t isBooted = 0;
  uint16_t startTime = millis();
  VL53L1_Error status;

  ESP_LOGD(TAG, "Waiting for boot with timeout: %d", this->timeout);
  while (!(isBooted & 1) && (millis() < this->timeout)) {
    status = this->sensor.GetBootState(&isBooted);
    delay(5);
    if (status != VL53L1_ERROR_NONE) {
      return status;
    }
    App.feed_wdt();
  }

  if (!(isBooted & 1)) {
    ESP_LOGW(TAG, "Timed out waiting for boot. state: %d", isBooted);
    return VL53L1_ERROR_TIME_OUT;
  }
  return VL53L1_ERROR_NONE;
}

void VL53L1X::set_ranging_mode(const RangingMode *mode) {
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Cannot set ranging mode while component is failed");
    return;
  }

  auto status = this->sensor.SetDistanceMode(mode->mode);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Could not set distance mode: %d, error code: %d", mode->mode, status);
  }

  status = this->sensor.SetTimingBudgetInMs(mode->timing_budget);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Could not set timing budget: %d ms, error code: %d", mode->timing_budget, status);
  }

  status = this->sensor.SetInterMeasurementInMs(mode->delay_between_measurements);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Could not set measurement delay: %d ms, error code: %d", mode->delay_between_measurements, status);
  }

  this->ranging_mode = mode;
  ESP_LOGI(TAG, "Set ranging mode: %s", mode->name);
}

optional<uint16_t> VL53L1X::read_distance(ROI *roi, VL53L1_Error &status) {
  if (this->is_failed()) {
    ESP_LOGW(TAG, "Cannot read distance while component is failed");
    return {};
  }

  ESP_LOGVV(TAG, "Beginning distance read");

  if (last_roi != nullptr && *roi != *last_roi) {
    status = this->sensor.SetROI(roi->width, roi->height);
    status += this->sensor.SetROICenter(roi->center);
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Could not set ROI, error code: %d", status);
      return {};
    }
    last_roi = roi;
  }

  status = this->sensor.StartRanging();

  // Wait for the measurement to be ready
  // TODO use interrupt_pin, if given, to await data ready instead of polling
  uint8_t dataReady = false;
  while (!dataReady) {
    status = this->sensor.CheckForDataReady(&dataReady);
    if (status != VL53L1_ERROR_NONE) {
      ESP_LOGE(TAG, "Failed to check if data is ready, error code: %d", status);
      return {};
    }
    delay(1);
    App.feed_wdt();
  }

  // Get the results
  uint16_t distance;
  status = this->sensor.GetDistanceInMm(&distance);
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Could not get distance, error code: %d", status);
    return {};
  }

  // After reading the results reset the interrupt to be able to take another measurement
  status = this->sensor.ClearInterrupt();
  status += this->sensor.StopRanging();
  if (status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Could not stop ranging, error code: %d", status);
    return {};
  }

  ESP_LOGV(TAG, "Finished distance read: %d", distance);
  return {distance};
}

}  // namespace vl53l1x
}  // namespace esphome
