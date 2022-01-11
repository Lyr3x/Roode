#pragma once
#include "VL53L1X_ULD.h"

namespace esphome {
namespace roode {

struct RangingConfig {
  explicit RangingConfig(uint16_t timing_budget)
      : mode{timing_budget == 15 ? Short : Long}, timing_budget{timing_budget} {}

  explicit RangingConfig(uint16_t timing_budget, EDistanceMode mode) : mode{mode}, timing_budget{timing_budget} {}

  EDistanceMode const mode;
  uint16_t const timing_budget;
  uint16_t const delay_between_measurements = timing_budget + 5;
};

namespace Ranging {
static const RangingConfig *Shortest = new RangingConfig(15);
static const RangingConfig *Short = new RangingConfig(20);
static const RangingConfig *Medium = new RangingConfig(33);
static const RangingConfig *MediumLong = new RangingConfig(50);
static const RangingConfig *Long = new RangingConfig(100);
static const RangingConfig *Longer = new RangingConfig(200);
static const RangingConfig *Longest = new RangingConfig(500);

static const RangingConfig *Custom(uint16_t timing_budget) { return new RangingConfig(timing_budget); };
static const RangingConfig *Custom(uint16_t timing_budget, EDistanceMode mode) {
  return new RangingConfig(timing_budget, mode);
};
}  // namespace Ranging

}  // namespace roode
}  // namespace esphome
