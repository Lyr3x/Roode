#pragma once
#include <math.h>

#include "VL53L1X_ULD.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/log.h"
#include "ranging.h"
#include "roi.h"

namespace esphome {
namespace vl53l1x {
static const char *const TAG = "VL53L1X";

/**
 * A wrapper for the VL53L1X, Time-of-Flight (ToF), laser-ranging sensor.
 * This stores user calibration info.
 */
class VL53L1X : public i2c::I2CDevice, public Component {
 public:
  void setup() override;
  void dump_config() override;
  /** This connects directly to a sensor */
  float get_setup_priority() const override { return setup_priority::DATA; };

  optional<uint16_t> read_distance(ROI *roi, VL53L1_Error &error);
  void set_ranging_mode(const RangingMode *mode);

  void set_xshut_pin(GPIOPin *pin) { this->xshut_pin = pin; }
  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin = pin; }
  optional<const RangingMode *> get_ranging_mode_override() { return this->ranging_mode_override; }
  void set_ranging_mode_override(const RangingMode *mode) { this->ranging_mode_override = {mode}; }
  void set_offset(int16_t val) { this->offset = val; }
  void set_xtalk(uint16_t val) { this->xtalk = val; }
  void set_timeout(uint16_t val) { this->timeout = val; }

 protected:
  VL53L1X_ULD sensor;
  optional<GPIOPin *> xshut_pin{};
  optional<InternalGPIOPin *> interrupt_pin{};
  const RangingMode *ranging_mode{};
  /** Mode from user config, which can be get/set independently of current mode */
  optional<const RangingMode *> ranging_mode_override{};
  optional<int16_t> offset{};
  optional<uint16_t> xtalk{};
  uint16_t timeout{};
  ROI *last_roi{};

  VL53L1_Error init();
  VL53L1_Error wait_for_boot();
};

}  // namespace vl53l1x
}  // namespace esphome
