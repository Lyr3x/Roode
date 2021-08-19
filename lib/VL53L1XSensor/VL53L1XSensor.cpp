#include <VL53L1XSensor.h>

VL53L1XSensor::VL53L1XSensor(int XSHUT, int I2C_ADDRESS)
{
    _XSHUT = XSHUT;
    _I2C_ADDRESS = I2C_ADDRESS;
}

void VL53L1XSensor::init()
{
    pinMode(XSHUT_PIN, OUTPUT);
    delay(100);
    dev1_sel
        Sensor->I2cDevAddr = _I2C_ADDRESS;
    checkDev();
    delay(1000);

    // ESP_LOGD("VL53L1X custom sensor", "Getting Device data");
    uint16_t wordData;
    VL53L1_RdWord(Sensor, 0x010F, &wordData);

    delay(1000);
    VL53L1_software_reset(Sensor);

    // ESP_LOGD("VL53L1X custom sensor", "Autonomous Ranging Test");

    status += VL53L1_WaitDeviceBooted(Sensor);
    status += VL53L1_DataInit(Sensor);
    status += VL53L1_StaticInit(Sensor);
    #ifndef INIT_WITH_SHORT
    status += VL53L1_SetDistanceMode(Sensor, VL53L1_DISTANCEMODE_LONG);
    #else
    status += VL53L1_SetDistanceMode(Sensor, VL53L1_DISTANCEMODE_SHORT);
    #endif
    status += VL53L1_SetMeasurementTimingBudgetMicroSeconds(Sensor, 10000); // 73Hz
    status += VL53L1_SetInterMeasurementPeriodMilliSeconds(Sensor, 15);
    if (status)
    {
        // ESP_LOGE("VL53L1X custom sensor", "StartMeasurement failed status: %d", status);
    }
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
        status += VL53L1_SetDistanceMode(Sensor, VL53L1_DISTANCEMODE_LONG);
    }
    else if (mode == MEDIUM_RANGE)
    {
        status += VL53L1_SetDistanceMode(Sensor, VL53L1_DISTANCEMODE_MEDIUM);
    }
    else if (mode == SHORT_RANGE)
    {
        status += VL53L1_SetDistanceMode(Sensor, VL53L1_DISTANCEMODE_SHORT);
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

int VL53L1XSensor::readRangeContinuoisMillimeters(VL53L1_UserRoi_t roiConfig, int delay_ms)
{
    status = VL53L1_SetUserROI(Sensor, &roiConfig);
    status = VL53L1_WaitMeasurementDataReady(Sensor);
    if (delay != 0)
    {
        delay(delay_ms);
    }
    if (!status)
        status = VL53L1_GetRangingMeasurementData(Sensor, &RangingData);
    VL53L1_clear_interrupt_and_enable_next_range(Sensor, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);
    if (status == 0)
        return RangingData.RangeMilliMeter;
    else
    {
        return -1;
    }
}

void VL53L1XSensor::setPresetMode(int mode)
{
    status += VL53L1_SetPresetMode(Sensor, VL53L1_PRESETMODE_LITE_RANGING);
}

void VL53L1XSensor::startMeasurement()
{
    VL53L1_StartMeasurement(Sensor);
}
void VL53L1XSensor::stopMeasurement()
{
    VL53L1_StopMeasurement(Sensor);
}

void VL53L1XSensor::checkDev()
{
    uint16_t wordData;
    VL53L1_RdWord(Sensor, 0x010F, &wordData);
    Serial.printf("DevAddr: 0x%X VL53L1X: 0x%X\n\r", Sensor->I2cDevAddr, wordData);
}

void VL53L1XSensor::setIntermeasurementPeriod(int period)
{
    VL53L1_SetInterMeasurementPeriodMilliSeconds(Sensor, 15);
}