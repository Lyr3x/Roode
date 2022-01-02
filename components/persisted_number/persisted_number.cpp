#include "persisted_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace number {

// Same as parent just changed log statement from debug to info
auto PersistedNumber::publish_state(float state) -> void {
    this->has_state_ = true;
    this->state = state;
    ESP_LOGI("number", "'%s': Sending state %f", this->get_name().c_str(), state);
    this->state_callback_.call(state);
}

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

}
}
