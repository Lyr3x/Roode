#pragma once
#include <math.h>
#include <Wire.h>

#include "VL53L1X_ULD.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/log.h"
#include "ranging.h"
#include "roi.h"

namespace esphome {
namespace roode {

/**
 * A wrapper for the VL53L1X, Time-of-Flight (ToF), laser-ranging sensor.
 * This stores user calibration info.
 */
class TofSensor : public Component {
 public:
  void setup() override;
  void dump_config() override;
  // After GPIO, but before default...Is this needed? not sure.
  float get_setup_priority() const override { return setup_priority::DATA; };

  optional<uint16_t> read_distance(ROI *roi, VL53L1_Error &error);
  void set_ranging_mode(const RangingMode *mode);

  void set_i2c_address(uint8_t val) { this->address = val; }
  void set_xshut_pin(InternalGPIOPin *pin) { this->xshut_pin = pin; }
  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin = pin; }
  optional<const RangingMode *> get_ranging_mode_override() { return this->ranging_mode_override; }
  void set_ranging_mode_override(const RangingMode *mode) { this->ranging_mode_override = {mode}; }
  void set_offset(int16_t val) { this->offset = val; }
  void set_xtalk(uint16_t val) { this->xtalk = val; }

 protected:
  VL53L1X_ULD sensor;
  optional<uint8_t> address{};
  optional<InternalGPIOPin *> xshut_pin{};
  optional<InternalGPIOPin *> interrupt_pin{};
  optional<const RangingMode *> ranging_mode_override{};
  optional<int16_t> offset{};
  optional<uint16_t> xtalk{};
};

}  // namespace roode
}  // namespace esphome
