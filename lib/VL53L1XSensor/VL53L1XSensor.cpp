#include <VL53L1XSensor.h>
VL53L1XSensor::VL53L1XSensor(int XSHUT, int I2C_ADDRESS)
{
    _XSHUT = XSHUT;
    _I2C_ADDRESS = I2C_ADDRESS;
}

void VL53L1XSensor::init()
{
    Serial.println(F("Init VL53L0X Sensor"));
    // Reset Sensor
    pinMode(_XSHUT, OUTPUT);
    digitalWrite(_XSHUT, LOW);
    delay(10);
    digitalWrite(_XSHUT, HIGH);
    _Sensor->I2cDevAddr = _I2C_ADDRESS;
    checkDev(_Sensor);
    status += VL53L1_WaitDeviceBooted(_Sensor);
    status += VL53L1_DataInit(_Sensor);
    status += VL53L1_StaticInit(_Sensor);
    setRangeMode(SENSOR_RANGE_MODE);
    setPresetMode(SENSOR_PRESET_MODE);
    status += VL53L1_SetMeasurementTimingBudgetMicroSeconds(_Sensor, 10000);
    status += VL53L1_SetInterMeasurementPeriodMilliSeconds(_Sensor, 15);
}

/**
 * @brief  Set the distance mode
 * @par Function Description
 * Set the distance mode to be used for the next ranging.<br>
 * The modes Short, Medium and Long are used to optimize the ranging accuracy
 * in a specific range of distance.<br> The user select one of these modes to
 * select the distance range.
 * @note This function doesn't Access to the device
 *
 * @warning This function should be called after @a VL53L1_SetPresetMode().

 * @param   Dev                   Device Handle
 * @param   DistanceMode          Distance mode to apply, valid values are:
 * @li VL53L1_DISTANCEMODE_SHORT
 * @li VL53L1_DISTANCEMODE_MEDIUM
 * @li VL53L1_DISTANCEMODE_LONG
 * @return  VL53L1_ERROR_NONE               Success
 * @return  VL53L1_ERROR_MODE_NOT_SUPPORTED This error occurs when DistanceMode
 *                                          is not in the supported list
 * @return  "Other error code"              See ::VL53L1_Error
 */

void VL53L1XSensor::setRangeMode(int mode)
{
    if (mode == LONG_RANGE)
    {
        status += VL53L1_SetDistanceMode(_Sensor, VL53L1_DISTANCEMODE_LONG);
    }
    else if (mode == MEDIUM_RANGE)
    {
        status += VL53L1_SetDistanceMode(_Sensor, VL53L1_DISTANCEMODE_MEDIUM);
    }
    else if (mode == SHORT_RANGE)
    {
        status += VL53L1_SetDistanceMode(_Sensor, VL53L1_DISTANCEMODE_SHORT);
    }
}

/**
 * @brief  Set a new Preset Mode
 * @par Function Description
 * Set device to a new Operating Mode (High speed ranging, Multi objects ...)
 *
 * @note This function doesn't Access to the device
 *
 * @warning This function change the timing budget to 16 ms and the inter-
 * measurement period to 1000 ms. Also the VL53L1_DISTANCEMODE_LONG is used.
 *
 * @param   Dev                   Device Handle
 * @param   PresetMode            New Preset mode to apply
 * <br>Valid values are:
 */
/**
 * @li VL53L1_PRESETMODE_LITE_RANGING
 * @li VL53L1_PRESETMODE_AUTONOMOUS
 * @li VL53L1_PRESETMODE_LOWPOWER_AUTONOMOUS
 */
/**
 *
 * @return  VL53L1_ERROR_NONE               Success
 * @return  VL53L1_ERROR_MODE_NOT_SUPPORTED This error occurs when PresetMode is
 *                                          not in the supported list
 */

void VL53L1XSensor::setPresetMode(int mode)
{
    status += VL53L1_SetPresetMode(_Sensor, VL53L1_PRESETMODE_LITE_RANGING);
}

uint16_t VL53L1XSensor::readData()
{
    return readRangeContinuousMillimeters();
}
void VL53L1XSensor::startContinuous()
{
    VL53L1_StartMeasurement(_Sensor);
}
void VL53L1XSensor::stopContinuous()
{
    VL53L1_StopMeasurement(_Sensor);
}

int VL53L1XSensor::calibration()
{
    int irValues[30] = {};
    uint16_t min = 0;

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
    // threshold = max + THRESHOLD_X;#

    Serial.print(F("standard deviation: "));
    Serial.println(sd);
    Serial.print(F("New threshold is: "));
    Serial.println(this->threshold);
    Serial.println(F("#### calibration done ####"));

    //send(thrMsg.set(threshold)); //REWORK

    return this->threshold;
}

uint16_t VL53L1XSensor::getThreshold()
{
    return this->threshold;
}

void VL53L1XSensor::checkDev(VL53L1_DEV Dev)
{
    uint16_t wordData;
    VL53L1_RdWord(Dev, 0x010F, &wordData);
    Serial.printf("DevAddr: 0x%X VL53L1X: 0x%X\n\r", Dev->I2cDevAddr, wordData);
}