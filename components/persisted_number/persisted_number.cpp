#include "persisted_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace number {

auto PersistedNumber::control(float newValue) -> void {
  this->publish_state(newValue);
  if (this->restore_value_) {
    this->pref_.save(&newValue);
  }
}

auto PersistedNumber::setup() -> void {
  float value;
  if (!this->restore_value_) {
    value = this->traits.get_min_value();
  } else {
    this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
    if (this->pref_.load(&value)) {
      ESP_LOGI("number", "'%s': Restored state %f", this->get_name().c_str(), value);
    } else {
      ESP_LOGI("number", "'%s': No previous state found", this->get_name().c_str());
      value = this->traits.get_min_value();
    }
  }
  this->publish_state(value);
}

}  // namespace number
}  // namespace esphome
