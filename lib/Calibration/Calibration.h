#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <math.h>
#include "esphome/core/log.h"
#include <VL53L1X.h>

/*
##### CALIBRATION ##### 
*/
static int DIST_THRESHOLD_MAX[] = {0, 0}; // treshold of the two zones
static int MIN_DISTANCE[] = {0, 0};
static int center[2] = {0, 0}; /* center of the two zones */
static int zone = 0;
static uint16_t distance = 0;

/*
Use the VL53L1X_SetTimingBudget function to set the TB in milliseconds. The TB values available are [15, 20,
33, 50, 100, 200, 500]. This function must be called after VL53L1X_SetDistanceMode.
Note: 15 ms only works with Short distance mode. 100 ms is the default value.
The TB can be adjusted to improve the standard deviation (SD) of the measurement. 
Increasing the TB, decreases the SD but increases the power consumption.
*/

static int delay_between_measurements = 0;
static int time_budget_in_ms = 0;

// this value has to be true if the sensor is oriented as in Duthdeffy's picture
static bool advised_orientation_of_the_sensor = true;

// this value has to be true if you don't need to compute the threshold every time the device is turned on
static bool save_calibration_result = true;

// parameters which define the time between two different measurements in longRange mode
static int delay_between_measurements_long = 50;
static int time_budget_in_ms_long = 33; // Works up to 3.1m increase to 140ms for 4m
static int delay_between_measurements_short = 25;
static int time_budget_in_ms_short = 15;

// value which defines the threshold which activates the short distance mode (the sensor supports it only up to a distance of 1300 mm)
static int short_distance_threshold = 1300;

float sum_zone_0 = 0;
float sum_zone_1 = 0;
int number_attempts = 20;
float average_zone_0 = 0;
float average_zone_1 = 0;
void roi_calibration(VL53L1X distanceSensor)
{
    // the value of the average distance is used for computing the optimal size of the ROI and consequently also the center of the two zones
    int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(average_zone_0, average_zone_1) / 1000)));
    delay(1000);
    int ROI_size = min(8, max(4, function_of_the_distance));
    id(ROI_width) = ROI_size;
    id(ROI_height) = ROI_size;
    if (average_zone_0 <= short_distance_threshold || average_zone_1 <= short_distance_threshold)
    {
        // we can use the short mode, which allows more precise measurements up to 1.3 meters
        time_budget_in_ms = time_budget_in_ms_short;
        delay_between_measurements = delay_between_measurements_short;
        distanceSensor.setDistanceMode(VL53L1X::Short);
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
        distanceSensor.setROISize(id(ROI_width), id(ROI_height));
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_0 = sum_zone_0 + distance;
        zone++;
        zone = zone % 2;

        // increase sum of values in Zone 1
        distanceSensor.setROISize(id(ROI_width), id(ROI_height));
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_1 = sum_zone_1 + distance;
        zone++;
        zone = zone % 2;
        yield();
    }
    average_zone_0 = sum_zone_0 / number_attempts;
    average_zone_1 = sum_zone_1 / number_attempts;
    EEPROM.write(3, ROI_size);
}
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
    delay(500);

    zone = 0;

    for (int i = 0; i < number_attempts; i++)
    {
        // increase sum of values in Zone 1
        distanceSensor.setROISize(id(ROI_width), id(ROI_height));
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_0 = sum_zone_0 + distance;
        zone++;
        zone = zone % 2;

        // increase sum of values in Zone 2
        distanceSensor.setROISize(id(ROI_width), id(ROI_height));
        distanceSensor.setROICenter(center[zone]);
        distanceSensor.startContinuous(delay_between_measurements);
        distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
        distance = distanceSensor.read();
        distanceSensor.stopContinuous();
        sum_zone_1 = sum_zone_1 + distance;
        zone++;
        zone = zone % 2;
        yield();
    }
    // after we have computed the sum for each zone, we can compute the average distance of each zone
    average_zone_0 = sum_zone_0 / number_attempts;
    average_zone_1 = sum_zone_1 / number_attempts;

    if (id(ROI_CALIBRATION))
    {
        roi_calibration(distanceSensor);
    }

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
    EEPROM.write(4, hundred_threshold_zone_0);
    EEPROM.write(5, unit_threshold_zone_0);
    EEPROM.write(6, hundred_threshold_zone_1);
    EEPROM.write(7, unit_threshold_zone_1);
    EEPROM.commit();
}

void calibration_boot(VL53L1X distanceSensor)
{
    ESP_LOGI("Calibration", "#### calibration started ####");
    if (save_calibration_result)
    {
        // if possible, we take the old values of the zones contained in the EEPROM memory
        if (EEPROM.read(0) == 1)
        {
            // we have data in the EEPROM
            ESP_LOGI("Calibration", "EEPROM is not empty");
            center[0] = EEPROM.read(1);
            center[1] = EEPROM.read(2);
            id(ROI_height) = EEPROM.read(3);
            id(ROI_width) = EEPROM.read(3);
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
#endif