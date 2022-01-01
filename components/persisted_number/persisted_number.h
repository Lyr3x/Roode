#pragma once

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace number {

class PersistedNumber : public number::Number, public Component {
public:
    float get_setup_priority() const override { return setup_priority::HARDWARE; }
    void set_restore_value(bool restore) { this->restore_value_ = restore; }
    void setup() override;
    void publish_state(float state);

protected:
    void control(float value) override;

    bool restore_value_{false};
    ESPPreferenceObject pref_;
};

} // namespace roode
} // namespace esphome
