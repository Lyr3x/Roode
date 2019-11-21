#ifndef CALIBRATION_HPP
#define CALIBRATION_HPP
#include <Config.h>
#include <Math.h>
#include "VL53L1XSensor.h"

VL53L1_UserRoi_t leftRoiConfig = {10, 15, 15, 0}; //TopLeftX, TopLeftY, BotRightX, BotRightY
int calculateStandardDeviation(int irValues[]);
int calibration(VL53L1XSensor::VL53L1XSensor Sensor)
{

    int irValues[30] = {};
    uint16_t min = 0;

    auto n = 0;
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
        delay(10);
        auto sensor_value = Sensor.readRangeContinuoisMillimeters(&leftRoiConfig);

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
    Sensor.threshold = min - sd;

    // Serial.print("standard deviation: " + threshold);
    // threshold = max + THRESHOLD_X;#

    Serial.print(F("standard deviation: "));
    Serial.println(sd);
    Serial.print(F("New threshold is: "));
    Serial.println(Sensor.threshold);
    Serial.println(F("#### calibration done ####"));

    //send(thrMsg.set(threshold)); //REWORK

    return Sensor.threshold;
}
#endif