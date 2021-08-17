#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <Config.h>
#include <Math.h>
#include "esphome/core/log.h"

int calculateStandardDeviation(int irValues[])
{
    auto sumOfValues = 0;
    auto arrLength = 0;
    for (int i = 0; i < 30; i++)
    {
        if (irValues[i] != 0)
        {
            sumOfValues += irValues[i];
            arrLength++;
        }
    }

    auto meanValue = sumOfValues / arrLength;

    auto standardDeviation = 0;
    for (int i = 0; i < arrLength; ++i)
    {
        standardDeviation += pow(irValues[i] - meanValue, 2);
    }
    standardDeviation /= arrLength;

    standardDeviation = sqrt(standardDeviation);

    return standardDeviation;
}

void calibration(VL53L1XSensor Sensor)
{
    int irValues[30] = {};
    uint16_t min = 0;
    auto n = 0;
    Sensor.readRangeContinuoisMillimeters(roiConfig1, 100);
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
        auto sensor_value = Sensor.readRangeContinuoisMillimeters(roiConfig1);

        // #ifdef MY_DEBUG
        ESP_LOGD("VL53L1X custom sensor", "sensor_value: %d", sensor_value);
        // #endif
        //calculate the max without jumps for the room sensor
        if ((sensor_value < min) || ((sensor_value - min) == sensor_value))
        {
            ESP_LOGD("VL53L1X custom sensor", "sensor_value: %d", sensor_value);
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

// Serial.print("standard deviation: " + threshold);
// threshold = max + THRESHOLD_X;#
#undef DIST_THRESHOLD_MAX
#define DIST_THRESHOLD_MAX min - sd
    ESP_LOGI("VL53L1X custom sensor", "standard deviation: %d", sd);
    ESP_LOGI("VL53L1X custom sensor", "new threshold: %d", DIST_THRESHOLD_MAX);
    ESP_LOGI("VL53L1X custom sensor", "#### calibration done ####");
}
#endif