#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <Config.h>
#include <math.h>
#include "esphome/core/log.h"

/*
##### CALIBRATION ##### 
*/
#ifdef CALIBRATIONV2

static int delay_between_measurements = 0;
static int time_budget_in_ms = 0;
const int threshold_percentage = 80;

// this value has to be true if the sensor is oriented like this: | -> | 
static bool advised_orientation_of_the_sensor = true;

// this value has to be true if you don't need to compute the threshold every time the device is turned on
static bool save_calibration_result = true;

// parameters which define the time between two different measurements in longRange mode
static int delay_between_measurements_long = 55;
static int time_budget_in_ms_long = 50;

// parameters which define the time between two different measurements in longRange mode
static int time_budget_in_ms_short = 20;
static int delay_between_measurements_short = 22;

// value which defines the threshold which activates the short distance mode (the sensor supports it only up to a distance of 1300 mm)
static int short_distance_threshold = 1300;

/*
##### CALIBRATION END ##### 
*/
#endif //#ifdef CALIBRATIONV2

#ifdef CALIBRATION
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

//Calibration v1
void calibration(VL53L1XSensor Sensor)
{
    ESP_LOGI("VL53L1X custom sensor", "#### calibration started ####");
    int irValues[30] = {};
    uint16_t min = 0;
    auto n = 0;
    Sensor.setROI(ROI_height, ROI_width, center[Zone]);
    delay(delay_between_measurements);
    Sensor.setTimingBudgetInMs(time_budget_in_ms);
    Sensor.startRanging();
    distance = Sensor.getDistance();
    Sensor.stopRanging();
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
        auto sensor_value = Sensor.getDistance();

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

#endif //#ifdef CALIBRATION

#ifdef CALIBRATIONV2
void calibration(SFEVL53L1X Sensor)
{
    ESP_LOGI("VL53L1X custom sensor", "#### calibration started ####");
    /* 
    * 100 measurements for each of the defined tones
    * time_budget_in_ms_long = 100ms for increased precision
    */
    time_budget_in_ms = time_budget_in_ms_long;
    delay_between_measurements = delay_between_measurements_long;
    center[0] = 167;
    center[1] = 231;
    ROI_height = 16;
    ROI_width = 8;
    delay(500);

    Zone = 0;
    float sum_zone_0 = 0;
    float sum_zone_1 = 0;
    uint16_t distance;
    int number_attempts = 20;
    for (int i = 0; i < number_attempts; i++)
    {
        // increase sum of values in Zone 1
        Sensor.setROI(ROI_height, ROI_width, center[Zone]);
        delay(delay_between_measurements);
        Sensor.setTimingBudgetInMs(time_budget_in_ms);
        Sensor.startRanging();
        distance = Sensor.getDistance();
        Sensor.stopRanging();
        sum_zone_0 = sum_zone_0 + distance;

        // Update the zone
        Zone++;
        Zone = Zone % 2;

        // increase sum of values in Zone 2
        Sensor.setROI(ROI_height, ROI_width, center[Zone]);
        delay(delay_between_measurements);
        Sensor.setTimingBudgetInMs(time_budget_in_ms);
        Sensor.startRanging();
        distance = Sensor.getDistance();
        Sensor.stopRanging();

        sum_zone_1 = sum_zone_1 + distance;
        // Update the zone
        Zone++;
        Zone = Zone % 2;
    }
    // after we have computed the sum for each zone, we can compute the average distance of each zone
    float average_zone_0 = sum_zone_0 / number_attempts;
    float average_zone_1 = sum_zone_1 / number_attempts;
    // the value of the average distance is used for computing the optimal size of the ROI and the center of the two zones
    int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(average_zone_0, average_zone_1) / 1000)));
    delay(1000);
    int ROI_size = min(8, max(4, function_of_the_distance));
    ROI_width = ROI_size;
    ROI_height = ROI_size;
    if (average_zone_0 <= short_distance_threshold || average_zone_1 <= short_distance_threshold)
    {
        // we can use the short mode, which allows more precise measurements up to 1300 meters
        Sensor.setIntermeasurementPeriod(time_budget_in_ms_short);
        Sensor.setDistanceModeShort();
        time_budget_in_ms = time_budget_in_ms_short;
        delay_between_measurements = delay_between_measurements_short;
    }
    delay(250);

    // now we set the position of the center of the two zones
    if (advised_orientation_of_the_sensor)
    {

        switch (ROI_size)
        {
        case 4:
            center[0] = 150;
            center[1] = 247;
            break;
        case 5:
            center[0] = 159;
            center[1] = 239;
            break;
        case 6:
            center[0] = 159;
            center[1] = 239;
            break;
        case 7:
            center[0] = 167;
            center[1] = 231;
            break;
        case 8:
            center[0] = 167;
            center[1] = 231;
            break;
        }
    }
    else
    {
        switch (ROI_size)
        {
        case 4:
            center[0] = 193;
            center[1] = 58;
            break;
        case 5:
            center[0] = 194;
            center[1] = 59;
            break;
        case 6:
            center[0] = 194;
            center[1] = 59;
            break;
        case 7:
            center[0] = 195;
            center[1] = 60;
            break;
        case 8:
            center[0] = 195;
            center[1] = 60;
            break;
        }
    }
    delay(2000);
    // we will now repeat the calculations necessary to define the thresholds with the updated zones
    Zone = 0;
    sum_zone_0 = 0;
    sum_zone_1 = 0;
    for (int i = 0; i < number_attempts; i++)
    {
        // increase sum of values in Zone 0
        Sensor.setROI(ROI_height, ROI_width, center[Zone]);
        delay(delay_between_measurements);
        Sensor.setTimingBudgetInMs(time_budget_in_ms);
        Sensor.startRanging();
        distance = Sensor.getDistance();
        Sensor.stopRanging();
        sum_zone_0 = sum_zone_0 + distance;
        Zone++;
        Zone = Zone % 2;

        // increase sum of values in Zone 1
        Sensor.setROI(ROI_height, ROI_width, center[Zone]);
        delay(delay_between_measurements);
        Sensor.setTimingBudgetInMs(time_budget_in_ms);
        Sensor.startRanging();
        distance = Sensor.getDistance();
        Sensor.stopRanging();
        sum_zone_1 = sum_zone_1 + distance;
        Zone++;
        Zone = Zone % 2;
    }
    average_zone_0 = sum_zone_0 / number_attempts;
    average_zone_1 = sum_zone_1 / number_attempts;
    // Decimal values are not used for the threshold
    float threshold_zone_0 = average_zone_0 * threshold_percentage / 100;
    float threshold_zone_1 = average_zone_1 * threshold_percentage / 100;

    DIST_THRESHOLD_MAX[0] = threshold_zone_0;
    DIST_THRESHOLD_MAX[1] = threshold_zone_1;
    delay(2000);

    // we now save the values into the EEPROM memory
    int hundred_threshold_zone_0 = threshold_zone_0 / 100;
    int hundred_threshold_zone_1 = threshold_zone_1 / 100;
    int unit_threshold_zone_0 = threshold_zone_0 - 100 * hundred_threshold_zone_0;
    int unit_threshold_zone_1 = threshold_zone_1 - 100 * hundred_threshold_zone_1;
    ESP_LOGI("VL53L1X custom sensor", "New threshold zone1: %d", hundred_threshold_zone_0);
    ESP_LOGI("VL53L1X custom sensor", "New threshold zone2: %d", hundred_threshold_zone_1);

    EEPROM.write(0, 1);
    EEPROM.write(1, center[0]);
    EEPROM.write(2, center[1]);
    EEPROM.write(3, ROI_size);
    EEPROM.write(4, hundred_threshold_zone_0);
    EEPROM.write(5, unit_threshold_zone_0);
    EEPROM.write(6, hundred_threshold_zone_1);
    EEPROM.write(7, unit_threshold_zone_1);
    EEPROM.commit();

    ESP_LOGI("VL53L1X custom sensor", "#### calibration finished ####");
}

void calibration_boot(SFEVL53L1X Sensor)
{
    if (save_calibration_result)
    {
        // if possible, we take the old values of the zones contained in the EEPROM memory
        if (EEPROM.read(0) == 1)
        {
            // EEPROM is not empty
            center[0] = EEPROM.read(1);
            center[1] = EEPROM.read(2);
            ROI_height = EEPROM.read(3);
            ROI_width = EEPROM.read(3);
            DIST_THRESHOLD_MAX[0] = EEPROM.read(4) * 100 + EEPROM.read(5);
            DIST_THRESHOLD_MAX[1] = EEPROM.read(6) * 100 + EEPROM.read(7);

            // if the distance measured is smaller then 1300mm, then we can use the short range mode of the sensor
            if (min(DIST_THRESHOLD_MAX[0], DIST_THRESHOLD_MAX[1]) <= short_distance_threshold)
            {
                Sensor.setIntermeasurementPeriod(time_budget_in_ms_short);
                Sensor.setDistanceModeShort();
                time_budget_in_ms = time_budget_in_ms_short;
                delay_between_measurements = delay_between_measurements_short;
            }
            else
            {
                Sensor.setIntermeasurementPeriod(time_budget_in_ms_long);
                Sensor.setDistanceModeLong();
                time_budget_in_ms = time_budget_in_ms_long;
                delay_between_measurements = delay_between_measurements_long;
            }
        }
        else
        {
            // EEPROM is empty
            ESP_LOGI("VL53L1X custom sensor", "#### calibration on boot started ####");
            calibration(Sensor);
        }
    }
    else
        ESP_LOGI("VL53L1X custom sensor", "#### calibration on boot started ####");
    calibration(Sensor);
    ESP_LOGI("VL53L1X custom sensor", "#### calibration on boot done ####");
}
#endif //#ifdef CALIBRATIONV2
#endif
