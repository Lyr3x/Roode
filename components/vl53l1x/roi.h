#pragma once

namespace esphome {
namespace vl53l1x {

struct ROI {
  uint8_t width;
  uint8_t height;
  uint8_t center;
  void set_width(uint8_t val) { this->width = val; }
  void set_height(uint8_t val) { this->height = val; }
  void set_center(uint8_t val) { this->center = val; }
};

}  // namespace vl53l1x
}  // namespace esphome
