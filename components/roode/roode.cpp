#include "roode.h"

namespace esphome {
namespace roode {
void Roode::dump_config() {
  ESP_LOGCONFIG(TAG, "Roode:");
  ESP_LOGCONFIG(TAG, "  Sample size: %d", samples);
  ESP_LOGCONFIG(TAG, "  Path tracking timeout: %lu ms", path_tracking_timeout_ms_);
  ESP_LOGCONFIG(TAG, "  Adaptive thresholds: %s", adaptive_threshold_enabled_ ? "enabled" : "disabled");
  if (adaptive_threshold_enabled_) {
    ESP_LOGCONFIG(TAG, "    Update interval: %lu ms", adaptive_threshold_interval_ms_);
    ESP_LOGCONFIG(TAG, "    Smoothing alpha: %.2f", adaptive_threshold_alpha_);
  }
  LOG_UPDATE_INTERVAL(this);
  entry->dump_config();
  exit->dump_config();
}

void Roode::setup() {
  ESP_LOGI(SETUP, "Booting Roode %s", VERSION);
  if (version_sensor != nullptr) {
    version_sensor->publish_state(VERSION);
  }
  ESP_LOGI(SETUP, "Using sampling with sampling size: %d", samples);

  if (this->distanceSensor->is_failed()) {
    this->mark_failed();
    ESP_LOGE(TAG, "Roode cannot be setup without a valid VL53L1X sensor");
    return;
  }

  // Initialize timestamps for new features
  uint32_t now = millis();
  last_state_change_time_ = now;
  last_adaptive_update_time_ = now;
  zones_empty_since_ = now;

  calibrate_zones();

  if (adaptive_threshold_enabled_) {
    ESP_LOGI(SETUP, "Adaptive thresholds enabled (interval: %lu ms, alpha: %.2f)",
             adaptive_threshold_interval_ms_, adaptive_threshold_alpha_);
  }
  ESP_LOGI(SETUP, "Path tracking timeout: %lu ms", path_tracking_timeout_ms_);
}

void Roode::update() {
  if (distance_entry != nullptr) {
    distance_entry->publish_state(entry->getDistance());
  }
  if (distance_exit != nullptr) {
    distance_exit->publish_state(exit->getDistance());
  }
}

void Roode::loop() {
  this->current_zone->readDistance(distanceSensor);
  path_tracking(this->current_zone);
  handle_sensor_status();

  // Adaptive threshold update when both zones are empty
  if (adaptive_threshold_enabled_) {
    updateAdaptiveThresholds();
  }

  this->current_zone = this->current_zone == this->entry ? this->exit : this->entry;
}

bool Roode::handle_sensor_status() {
  bool check_status = false;
  if (last_sensor_status != sensor_status && sensor_status == VL53L1_ERROR_NONE) {
    if (status_sensor != nullptr) {
      status_sensor->publish_state(sensor_status);
    }
    check_status = true;
  }
  if (sensor_status < 28 && sensor_status != VL53L1_ERROR_NONE) {
    ESP_LOGE(TAG, "Ranging failed with an error. status: %d", sensor_status);
    status_sensor->publish_state(sensor_status);
    check_status = false;
  }

  last_sensor_status = sensor_status;
  sensor_status = VL53L1_ERROR_NONE;
  return check_status;
}

void Roode::resetPathTracking() {
  path_track_[0] = 0;
  path_track_[1] = 0;
  path_track_[2] = 0;
  path_track_[3] = 0;
  path_track_filling_size_ = 1;
  left_previous_status_ = NOBODY;
  right_previous_status_ = NOBODY;
  last_state_change_time_ = millis();
  ESP_LOGD(TAG, "Path tracking state reset (timeout or completion)");
}

void Roode::path_tracking(Zone *zone) {
  int CurrentZoneStatus = NOBODY;
  int AllZonesCurrentStatus = 0;
  int AnEventHasOccured = 0;
  uint32_t now = millis();

  // Timeout check: reset state if no activity for too long
  // This prevents stuck states when someone enters halfway and turns back
  if (path_track_filling_size_ > 1 && path_tracking_timeout_ms_ > 0) {
    if ((now - last_state_change_time_) > path_tracking_timeout_ms_) {
      ESP_LOGW(TAG, "Path tracking timeout after %lu ms - resetting state (incomplete crossing)",
               now - last_state_change_time_);
      if (entry_exit_event_sensor != nullptr) {
        entry_exit_event_sensor->publish_state("Timeout");
      }
      resetPathTracking();
      return;
    }
  }

  // PathTrack algorithm
  if (zone->getMinDistance() < zone->threshold->max && zone->getMinDistance() > zone->threshold->min) {
    // Someone is in the sensing area
    CurrentZoneStatus = SOMEONE;
    if (presence_sensor != nullptr) {
      presence_sensor->publish_state(true);
    }
  }

  // left zone
  if (zone == (this->invert_direction_ ? this->exit : this->entry)) {
    if (CurrentZoneStatus != left_previous_status_) {
      // event in left zone has occured
      AnEventHasOccured = 1;

      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 1;
      }
      // need to check right zone as well ...
      if (right_previous_status_ == SOMEONE) {
        // event in right zone has occured
        AllZonesCurrentStatus += 2;
      }
      // remember for next time
      left_previous_status_ = CurrentZoneStatus;
    }
  }
  // right zone
  else {
    if (CurrentZoneStatus != right_previous_status_) {
      // event in right zone has occured
      AnEventHasOccured = 1;
      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 2;
      }
      // need to check left zone as well ...
      if (left_previous_status_ == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 1;
      }
      // remember for next time
      right_previous_status_ = CurrentZoneStatus;
    }
  }

  // if an event has occured
  if (AnEventHasOccured) {
    last_state_change_time_ = now;  // Update timestamp on state change
    ESP_LOGD(TAG, "Event has occured, AllZonesCurrentStatus: %d", AllZonesCurrentStatus);
    if (path_track_filling_size_ < 4) {
      path_track_filling_size_++;
    }

    // if nobody anywhere lets check if an exit or entry has happened
    if ((left_previous_status_ == NOBODY) && (right_previous_status_ == NOBODY)) {
      ESP_LOGD(TAG, "Nobody anywhere, AllZonesCurrentStatus: %d", AllZonesCurrentStatus);
      // check exit or entry only if path_track_filling_size_ is 4 (for example 0 1
      // 3 2) and last event is 0 (nobobdy anywhere)
      if (path_track_filling_size_ == 4) {
        // check exit or entry. no need to check path_track_[0] == 0 , it is
        // always the case

        if ((path_track_[1] == 1) && (path_track_[2] == 3) && (path_track_[3] == 2)) {
          // This an exit
          ESP_LOGI("Roode pathTracking", "Exit detected.");

          this->updateCounter(-1);
          if (entry_exit_event_sensor != nullptr) {
            entry_exit_event_sensor->publish_state("Exit");
          }
        } else if ((path_track_[1] == 2) && (path_track_[2] == 3) && (path_track_[3] == 1)) {
          // This an entry
          ESP_LOGI("Roode pathTracking", "Entry detected.");
          this->updateCounter(1);
          if (entry_exit_event_sensor != nullptr) {
            entry_exit_event_sensor->publish_state("Entry");
          }
        } else {
          // Incomplete or invalid sequence - log for debugging
          ESP_LOGD(TAG, "Invalid path sequence: [%d, %d, %d, %d]",
                   path_track_[0], path_track_[1], path_track_[2], path_track_[3]);
        }
      }

      resetPathTracking();
    } else {
      // update path_track_
      // example of path_track_ update
      // 0
      // 0 1
      // 0 1 3
      // 0 1 3 1
      // 0 1 3 3
      // 0 1 3 2 ==> if next is 0 : check if exit
      path_track_[path_track_filling_size_ - 1] = AllZonesCurrentStatus;
    }
  }
  if (presence_sensor != nullptr) {
    if (CurrentZoneStatus == NOBODY && left_previous_status_ == NOBODY && right_previous_status_ == NOBODY) {
      // nobody is in the sensing area
      presence_sensor->publish_state(false);
    }
  }
}
void Roode::updateCounter(int delta) {
  if (this->people_counter == nullptr) {
    return;
  }
  auto next = this->people_counter->state + (float) delta;
  ESP_LOGI(TAG, "Updating people count: %d", (int) next);
  auto call = this->people_counter->make_call();
  call.set_value(next);
  call.perform();
}
void Roode::recalibration() { calibrate_zones(); }

void Roode::updateAdaptiveThresholds() {
  uint32_t now = millis();
  bool both_zones_empty = !entry->isOccupied() && !exit->isOccupied();

  if (both_zones_empty) {
    // Track when zones became empty
    if (zones_were_occupied_) {
      zones_empty_since_ = now;
      zones_were_occupied_ = false;
    }

    // Only update if zones have been empty for at least the update interval
    // This ensures we're capturing true idle readings, not brief gaps during crossings
    uint32_t empty_duration = now - zones_empty_since_;
    if (empty_duration >= adaptive_threshold_interval_ms_) {
      // Check if enough time has passed since last update
      if ((now - last_adaptive_update_time_) >= adaptive_threshold_interval_ms_) {
        ESP_LOGD(TAG, "Updating adaptive thresholds (zones empty for %lu ms)", empty_duration);

        entry->updateAdaptiveThreshold(adaptive_threshold_alpha_);
        exit->updateAdaptiveThreshold(adaptive_threshold_alpha_);

        last_adaptive_update_time_ = now;

        // Publish updated threshold values if sensors are configured
        if (max_threshold_entry_sensor != nullptr) {
          max_threshold_entry_sensor->publish_state(entry->threshold->max);
        }
        if (max_threshold_exit_sensor != nullptr) {
          max_threshold_exit_sensor->publish_state(exit->threshold->max);
        }
        if (min_threshold_entry_sensor != nullptr) {
          min_threshold_entry_sensor->publish_state(entry->threshold->min);
        }
        if (min_threshold_exit_sensor != nullptr) {
          min_threshold_exit_sensor->publish_state(exit->threshold->min);
        }
      }
    }
  } else {
    zones_were_occupied_ = true;
  }
}

const RangingMode *Roode::determine_raning_mode(uint16_t average_entry_zone_distance,
                                                uint16_t average_exit_zone_distance) {
  uint16_t min = average_entry_zone_distance < average_exit_zone_distance ? average_entry_zone_distance
                                                                          : average_exit_zone_distance;
  uint16_t max = average_entry_zone_distance > average_exit_zone_distance ? average_entry_zone_distance
                                                                          : average_exit_zone_distance;
  if (min <= short_distance_threshold) {
    return Ranging::Short;
  }
  if (max > short_distance_threshold && min <= medium_distance_threshold) {
    return Ranging::Medium;
  }
  if (max > medium_distance_threshold && min <= medium_long_distance_threshold) {
    return Ranging::Long;
  }
  if (max > medium_long_distance_threshold && min <= long_distance_threshold) {
    return Ranging::Longer;
  }
  return Ranging::Longest;
}

void Roode::calibrate_zones() {
  ESP_LOGI(SETUP, "Calibrating sensor zones");

  entry->reset_roi(orientation_ == Parallel ? 167 : 195);
  exit->reset_roi(orientation_ == Parallel ? 231 : 60);

  calibrateDistance();

  entry->roi_calibration(entry->threshold->idle, exit->threshold->idle, orientation_);
  entry->calibrateThreshold(distanceSensor, number_attempts);
  exit->roi_calibration(entry->threshold->idle, exit->threshold->idle, orientation_);
  exit->calibrateThreshold(distanceSensor, number_attempts);

  publish_sensor_configuration(entry, exit, true);
  App.feed_wdt();
  publish_sensor_configuration(entry, exit, false);
  ESP_LOGI(SETUP, "Finished calibrating sensor zones");
}

void Roode::calibrateDistance() {
  auto *const initial = distanceSensor->get_ranging_mode_override().value_or(Ranging::Longest);
  distanceSensor->set_ranging_mode(initial);

  entry->calibrateThreshold(distanceSensor, number_attempts);
  exit->calibrateThreshold(distanceSensor, number_attempts);

  if (distanceSensor->get_ranging_mode_override().has_value()) {
    return;
  }
  auto *mode = determine_raning_mode(entry->threshold->idle, exit->threshold->idle);
  if (mode != initial) {
    distanceSensor->set_ranging_mode(mode);
  }
}

void Roode::publish_sensor_configuration(Zone *entry, Zone *exit, bool isMax) {
  if (isMax) {
    if (max_threshold_entry_sensor != nullptr) {
      max_threshold_entry_sensor->publish_state(entry->threshold->max);
    }

    if (max_threshold_exit_sensor != nullptr) {
      max_threshold_exit_sensor->publish_state(exit->threshold->max);
    }
  } else {
    if (min_threshold_entry_sensor != nullptr) {
      min_threshold_entry_sensor->publish_state(entry->threshold->min);
    }
    if (min_threshold_exit_sensor != nullptr) {
      min_threshold_exit_sensor->publish_state(exit->threshold->min);
    }
  }

  if (entry_roi_height_sensor != nullptr) {
    entry_roi_height_sensor->publish_state(entry->roi->height);
  }
  if (entry_roi_width_sensor != nullptr) {
    entry_roi_width_sensor->publish_state(entry->roi->width);
  }

  if (exit_roi_height_sensor != nullptr) {
    exit_roi_height_sensor->publish_state(exit->roi->height);
  }
  if (exit_roi_width_sensor != nullptr) {
    exit_roi_width_sensor->publish_state(exit->roi->width);
  }
}
}  // namespace roode
}  // namespace esphome