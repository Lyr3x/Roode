#pragma once
#include "VL53L1X_ULD.h"

namespace esphome {
namespace vl53l1x {

struct RangingMode {
  explicit RangingMode(const char * name, uint16_t timing_budget, EDistanceMode mode = EDistanceMode::Long)
      : name{name}, timing_budget{timing_budget}, mode{mode} {}

  const char *name;
  uint16_t const timing_budget;
  uint16_t const delay_between_measurements = timing_budget + 5;
  EDistanceMode const mode;
};

namespace Ranging {
// NOLINTBEGIN(cert-err58-cpp)
__attribute__((unused)) static const RangingMode *Shortest = new RangingMode("Shortest", 15, Short);
__attribute__((unused)) static const RangingMode *Short = new RangingMode("Short", 20);
__attribute__((unused)) static const RangingMode *Medium = new RangingMode("Medium", 33);
__attribute__((unused)) static const RangingMode *Long = new RangingMode("Long", 50);
__attribute__((unused)) static const RangingMode *Longer = new RangingMode("Longer", 100);
__attribute__((unused)) static const RangingMode *Longest = new RangingMode("Longest", 200);
// NOLINTEND(cert-err58-cpp)
}  // namespace Ranging

}  // namespace vl53l1x
}  // namespace esphome
