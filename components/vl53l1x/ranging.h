#pragma once
#include "VL53L1X_ULD.h"

namespace esphome {
namespace vl53l1x {

struct RangingMode {
  explicit RangingMode(uint16_t timing_budget, EDistanceMode mode = EDistanceMode::Long)
      : timing_budget{timing_budget}, mode{mode} {}

  uint16_t const timing_budget;
  uint16_t const delay_between_measurements = timing_budget + 5;
  EDistanceMode const mode;
};

namespace Ranging {
// NOLINTBEGIN(cert-err58-cpp)
__attribute__((unused)) static const RangingMode *Shortest = new RangingMode(15, Short);
__attribute__((unused)) static const RangingMode *Short = new RangingMode(20);
__attribute__((unused)) static const RangingMode *Medium = new RangingMode(33);
__attribute__((unused)) static const RangingMode *Long = new RangingMode(50);
__attribute__((unused)) static const RangingMode *Longer = new RangingMode(100);
__attribute__((unused)) static const RangingMode *Longest = new RangingMode(200);
// NOLINTEND(cert-err58-cpp)
}  // namespace Ranging

}  // namespace vl53l1x
}  // namespace esphome
