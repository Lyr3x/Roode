#include <VL53L1X.h>
class VL53L1XWrap
{
public:
  VL53L1XWrap(VL53L1X Sensor);
  void setAddress(uint8_t new_addr);
  bool init(bool io_2v8 = true);
  void startContinuous(uint32_t period_ms = 33);
  void stopContinuous();
  bool setDistanceMode(VL53L1X::DistanceMode mode);
  bool setMeasurementTimingBudget(uint32_t budget_us);
  uint16_t readRangeContinuousMillimeters(void);
  bool timeoutOccurred();
  void setTimeout(uint16_t timeout);

private:
  VL53L1X _Sensor;
};