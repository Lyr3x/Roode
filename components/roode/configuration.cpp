#include "configuration.h"

#include "roode.h"
/*
This Object is used to store the sensor configratuin like calibration data,
sensor mode, ROI settings etc.
*/
namespace esphome {
namespace roode {
void Configuration::setSensorMode(VL53L1X_ULD distanceSensor, int sensor_mode, int new_timing_budget) {
  distanceSensor.StopRanging();
  switch (sensor_mode) {
    case 0:  // short mode
      time_budget_in_ms = time_budget_in_ms_short;
      delay_between_measurements = time_budget_in_ms + 5;
      sensor_status = distanceSensor.SetDistanceMode(Short);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Short);
      }
      ESP_LOGI(SETUP, "Set short mode. timing_budget: %d", time_budget_in_ms);
      break;
    case 1:  // medium mode
      time_budget_in_ms = time_budget_in_ms_medium;
      delay_between_measurements = time_budget_in_ms + 5;
      sensor_status = distanceSensor.SetDistanceMode(Long);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
      }
      ESP_LOGI(SETUP, "Set medium mode. timing_budget: %d", time_budget_in_ms);
      break;
    case 2:  // medium_long mode
      time_budget_in_ms = time_budget_in_ms_medium_long;
      delay_between_measurements = time_budget_in_ms + 5;
      sensor_status = distanceSensor.SetDistanceMode(Long);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
      }
      ESP_LOGI(SETUP, "Set medium long range mode. timing_budget: %d", time_budget_in_ms);
      break;
    case 3:  // long mode
      time_budget_in_ms = time_budget_in_ms_long;
      delay_between_measurements = time_budget_in_ms + 5;
      sensor_status = distanceSensor.SetDistanceMode(Long);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
      }
      ESP_LOGI(SETUP, "Set long range mode. timing_budget: %d", time_budget_in_ms);
      break;
    case 4:  // max mode
      time_budget_in_ms = time_budget_in_ms_max;
      delay_between_measurements = time_budget_in_ms + 5;
      sensor_status = distanceSensor.SetDistanceMode(Long);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
      }
      ESP_LOGI(SETUP, "Set max range mode. timing_budget: %d", time_budget_in_ms);
      break;
    case 5:  // custom mode
      time_budget_in_ms = new_timing_budget;
      delay_between_measurements = new_timing_budget + 5;
      sensor_status = distanceSensor.SetDistanceMode(Long);
      if (sensor_status != VL53L1_ERROR_NONE) {
        ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
      }
      ESP_LOGI(SETUP, "Manually set custom range mode. timing_budget: %d", time_budget_in_ms);
      break;
    default:
      break;
  }
  sensor_status = distanceSensor.SetTimingBudgetInMs(time_budget_in_ms);
  if (sensor_status != 0) {
    ESP_LOGE(SETUP, "Could not set timing budget.  timing_budget: %d ms", time_budget_in_ms);
  }
}
}  // namespace roode
}  // namespace esphome