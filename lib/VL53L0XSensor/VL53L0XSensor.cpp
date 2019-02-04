#include <VL53L0XSensor.h>
#include <Configuration.h>
VL53L0XSensor::VL53L0XSensor(int XSHUT, int I2C_ADDRESS)
{
    _XSHUT = XSHUT;
    _I2C_ADDRESS = I2C_ADDRESS;
}

void VL53L0XSensor::init()
{
    Serial.println("Init VL53L0X Sensor");
    // Reset Sensor
    pinMode(_XSHUT, OUTPUT);
    digitalWrite(_XSHUT, LOW);
    delay(10);
    //Power up Sensor
    pinMode(_XSHUT, INPUT);
    delay(10);
    _Sensor.setAddress(_I2C_ADDRESS);
    _Sensor.init();
    _Sensor.setTimeout(500);
    _Sensor.startContinuous();
}

void VL53L0XSensor::setMode(int mode)
{
    if (mode == LONG_RANGE)
    {
        // lower the return signal rate limit (default is 0.25 MCPS)
        _Sensor.setSignalRateLimit(0.1);
        // increase laser pulse periods (defaults are 14 and 10 PCLKs)
        _Sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        _Sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    }
    else if (mode == HIGH_SPEED)
    {
        // reduce timing budget to 20 ms (default is about 33 ms)
        _Sensor.setMeasurementTimingBudget(20000);
    }
    else if (mode == HIGH_ACCURACY)
    {
        // increase timing budget to 200 ms
        _Sensor.setMeasurementTimingBudget(200000);
    }
}
int VL53L0XSensor::readData()
{
    return readRangeContinuousMillimeters();
}

int VL53L0XSensor::calibration()
{
    int irValues[30] = {};
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(0, 5);
    oled.setTextSize(1, 1);
    oled.println("### Calibrate IR ###");
#endif
    auto min = 0;

    auto n = 0;
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
        delay(10);
        auto sensor_value = _Sensor.readRangeContinuousMillimeters();

        // #ifdef MY_DEBUG
        Serial.println(sensor_value);
        // #endif
        //calculate the max without jumps for the room sensor
        if ((sensor_value < min) || ((sensor_value - min) == sensor_value))
        {
            Serial.println(sensor_value);
            min = sensor_value;
            if (n < 30)
            {
                irValues[n] = min;
                n++;
            }
        }
    }
    auto sd = 0;

    sd = calculateStandardDeviation(irValues);
    this->threshold = min - sd;

// Serial.print("standard deviation: " + threshold);
// threshold = max + THRESHOLD_X;
#ifdef USE_OLED
    oled.setCursor(15, 0);
    oled.print("Threshold: ");
    oled.println(threshold);
    wait(2000);
    oled.clear();
    oled.setCursor(10, 0);
    oled.setTextSize(1, 1);
    oled.println("Calibration done!");
    wait(2000);
#endif
    Serial.print("standard deviation: ");
    Serial.println(sd);
    Serial.print("New threshold is: ");
    Serial.println("#### calibration done ####");
    Serial.println(this->threshold);
    //send(thrMsg.set(threshold)); //REWORK

    return this->threshold;
}

int VL53L0XSensor::getThreshold()
{
    return this->threshold;
}

void VL53L0XSensor::startContinuous(uint32_t period_ms)
{
    return _Sensor.startContinuous();
}

void VL53L0XSensor::stopContinuous()
{
    return _Sensor.stopContinuous();
}

uint16_t VL53L0XSensor::readRangeContinuousMillimeters(void)
{
    return _Sensor.readRangeContinuousMillimeters();
}
uint16_t VL53L0XSensor::readRangeSingleMillimeters(void)
{
    return _Sensor.readRangeSingleMillimeters();
}
inline void VL53L0XSensor::setTimeout(uint16_t timeout) { return _Sensor.setTimeout(timeout); }
inline uint16_t VL53L0XSensor::getTimeout(void) { return _Sensor.getTimeout(); }
bool VL53L0XSensor::timeoutOccurred(void)
{
    return _Sensor.timeoutOccurred();
}
