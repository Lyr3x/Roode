#include <VL53L1XWrap.h>
VL53L1XWrap::VL53L1XWrap(VL53L1X Sensor)
{
    _Sensor = Sensor;
}

uint16_t VL53L1XWrap::readRangeContinuousMillimeters(void)
{
    _Sensor.read();
    return _Sensor.ranging_data.range_mm;
}

void VL53L1XWrap::startContinuous(uint32_t period_ms = 33)
{
    return _Sensor.startContinuous(period_ms);
}
void VL53L1XWrap::stopContinuous()
{
    return _Sensor.stopContinuous();
}

void VL53L1XWrap::setAddress(uint8_t new_addr)
{
    return _Sensor.setAddress(new_addr);
}
bool VL53L1XWrap::init(bool io_2v8 = true)
{
    return _Sensor.init(io_2v8 = true);
}

bool VL53L1XWrap::setDistanceMode(VL53L1X::DistanceMode mode)
{
    return _Sensor.setDistanceMode(mode);
}
bool VL53L1XWrap::setMeasurementTimingBudget(uint32_t budget_us)
{
    return _Sensor.setMeasurementTimingBudget(budget_us);
}

bool VL53L1XWrap::timeoutOccurred()
{
    return _Sensor.timeoutOccurred();
}

void VL53L1XWrap::setTimeout(uint16_t timeout)
{
    return _Sensor.setTimeout(timeout);
}