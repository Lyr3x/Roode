#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <Config.h>
#include <math.h>
#include "esphome/core/log.h"

/*
##### CALIBRATION ##### 
*/
#ifdef CALIBRATIONV2
static int DIST_THRESHOLD_MAX[] = {0, 0}; // treshold of the two zones
static int MIN_DISTANCE[] = {0, 0};
static int center[2] = {0, 0}; /* center of the two zones */
static int ROI_height = 0;
static int ROI_width = 0;
static int zone = 0;

static int delay_between_measurements = 0;
static int time_budget_in_ms = 0;

// this value has to be true if the sensor is oriented as in Duthdeffy's picture
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

#endif //#ifdef CALIBRATION

#ifdef CALIBRATIONV2
void calibration(VL53L1X distanceSensor)
{
    // the sensor does 100 measurements for each zone (zones are predefined)
    time_budget_in_ms = time_budget_in_ms_long;
    delay_between_measurements = delay_between_measurements_long;
    distanceSensor.startContinuous(delay_between_measurements);
    distanceSensor.setDistanceMode(VL53L1X::Long);
    distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
    center[0] = 167;
    center[1] = 231;
    ROI_height = 16;
    ROI_width = 8;
    delay(500);

    zone = 0;
    float sum_zone_0 = 0;
    float sum_zone_1 = 0;
    uint16_t distance;
    int number_attempts = 20;
    for (int i = 0; i < number_attempts; i++)
    {
        // increase sum of values in Zone 1
        distanceSensor.setROISize(ROI_width, ROI_height);
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_0 = sum_zone_0 + distance;
        zone++;
        zone = zone % 2;

        // increase sum of values in Zone 2
        distanceSensor.setROISize(ROI_width, ROI_height);
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_1 = sum_zone_1 + distance;
        zone++;
        zone = zone % 2;
    }
    // after we have computed the sum for each zone, we can compute the average distance of each zone
    float average_zone_0 = sum_zone_0 / number_attempts;
    float average_zone_1 = sum_zone_1 / number_attempts;
    // the value of the average distance is used for computing the optimal size of the ROI and consequently also the center of the two zones
    int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(average_zone_0, average_zone_1) / 1000)));
    delay(1000);
    int ROI_size = min(8, max(4, function_of_the_distance));
    ROI_width = ROI_size;
    ROI_height = ROI_size;
    if (average_zone_0 <= short_distance_threshold || average_zone_1 <= short_distance_threshold)
    {
        // we can use the short mode, which allows more precise measurements up to 1.3 meters
        distanceSensor.setDistanceMode(VL53L1X::Short);
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
    zone = 0;
    sum_zone_0 = 0;
    sum_zone_1 = 0;
    for (int i = 0; i < number_attempts; i++)
    {
        // increase sum of values in Zone 0
        distanceSensor.setROISize(ROI_width, ROI_height);
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_0 = sum_zone_0 + distance;
        zone++;
        zone = zone % 2;

        // increase sum of values in Zone 1
        distanceSensor.setROISize(ROI_width, ROI_height);
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_1 = sum_zone_1 + distance;
        zone++;
        zone = zone % 2;
    }
    average_zone_0 = sum_zone_0 / number_attempts;
    average_zone_1 = sum_zone_1 / number_attempts;
    float threshold_zone_0 = average_zone_0 * id(threshold_percentage) / 100; // they can be int values, as we are not interested in the decimal part when defining the threshold
    float threshold_zone_1 = average_zone_1 * id(threshold_percentage) / 100;

    DIST_THRESHOLD_MAX[0] = threshold_zone_0;
    DIST_THRESHOLD_MAX[1] = threshold_zone_1;

    // we now save the values into the EEPROM memory
    int hundred_threshold_zone_0 = threshold_zone_0 / 100;
    int hundred_threshold_zone_1 = threshold_zone_1 / 100;
    int unit_threshold_zone_0 = threshold_zone_0 - 100 * hundred_threshold_zone_0;
    int unit_threshold_zone_1 = threshold_zone_1 - 100 * hundred_threshold_zone_1;
    ESP_LOGI("VL53L1X custom sensor", "Threshold zone1: %d", threshold_zone_0);
    ESP_LOGI("VL53L1X custom sensor", "Threshold zone2: %d", threshold_zone_1);
    delay(2000);

    EEPROM.write(0, 1);
    EEPROM.write(1, center[0]);
    EEPROM.write(2, center[1]);
    EEPROM.write(3, ROI_size);
    EEPROM.write(4, hundred_threshold_zone_0);
    EEPROM.write(5, unit_threshold_zone_0);
    EEPROM.write(6, hundred_threshold_zone_1);
    EEPROM.write(7, unit_threshold_zone_1);
    EEPROM.commit();
}

void calibration_boot(VL53L1X distanceSensor)
{
    ESP_LOGI("VL53L1X custom sensor", "#### calibration started ####");
    if (save_calibration_result)
    {
        // if possible, we take the old values of the zones contained in the EEPROM memory
        // client.publish(mqtt_serial_publish_distance_ch, "save calibration result true");
        if (EEPROM.read(0) == 1)
        {
            // we have data in the EEPROM
            //   client.publish(mqtt_serial_publish_distance_ch, "EEPROM memroy not empty");
            center[0] = EEPROM.read(1);
            center[1] = EEPROM.read(2);
            ROI_height = EEPROM.read(3);
            ROI_width = EEPROM.read(3);
            DIST_THRESHOLD_MAX[0] = EEPROM.read(4) * 100 + EEPROM.read(5);
            DIST_THRESHOLD_MAX[1] = EEPROM.read(6) * 100 + EEPROM.read(7);

            // if the distance measured is small, then we can use the short range mode of the sensor
            if (min(DIST_THRESHOLD_MAX[0], DIST_THRESHOLD_MAX[1]) <= short_distance_threshold)
            {
                time_budget_in_ms = time_budget_in_ms_short;
                delay_between_measurements = delay_between_measurements_short;
                distanceSensor.setDistanceMode(VL53L1X::Short);
                distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
            }
            else
            {
                time_budget_in_ms = time_budget_in_ms_long;
                delay_between_measurements = delay_between_measurements_long;
                distanceSensor.setDistanceMode(VL53L1X::Long);
                distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
            }
            //   client.publish(mqtt_serial_publish_distance_ch, "All values updated");
        }
        else
        {
            // there are no data in the EEPROM memory
            calibration(distanceSensor);
        }
    }
    else
        calibration(distanceSensor);
    ESP_LOGI("VL53L1X custom sensor", "#### calibration done ####");
}
#endif //#ifdef CALIBRATIONV2
#endif
