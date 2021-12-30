#include "esphome/core/log.h"
#include "roode.h"
namespace esphome
{
    namespace roode
    {
        static const char *const TAG = "Roode";
        static const char *const SETUP = "Setup";
        static const char *const CALIBRATION = "Calibration";
        void Roode::dump_config()
        {
            ESP_LOGCONFIG(TAG, "dump config:");
            LOG_I2C_DEVICE(this);

            LOG_UPDATE_INTERVAL(this);
        }
        void Roode::setup()
        {
            ESP_LOGI(SETUP, "Booting Roode %s", VERSION);
            if (version_sensor != nullptr)
            {
                version_sensor->publish_state(VERSION);
            }
            EEPROM.begin(EEPROM_SIZE);
            Wire.begin();
            Wire.setClock(400000);

            // Initialize the sensor, give the special I2C_address to the Begin function
            // Set a different I2C address
            // This address is stored as long as the sensor is powered. To revert this change you can unplug and replug the power to the sensor
            distanceSensor.SetI2CAddress(VL53L1X_ULD_I2C_ADDRESS);

            sensor_status = distanceSensor.Begin(VL53L1X_ULD_I2C_ADDRESS);
            if (sensor_status != VL53L1_ERROR_NONE)
            {
                // If the sensor could not be initialized print out the error code. -7 is timeout
                ESP_LOGE(SETUP, "Could not initialize the sensor, error code: %d", sensor_status);
                while (1)
                {
                }
            }
            if (sensor_offset_calibration_ != -1)
            {
                ESP_LOGI(CALIBRATION, "Setting sensor offset calibration to %d", sensor_offset_calibration_);
                sensor_status = distanceSensor.SetOffsetInMm(sensor_offset_calibration_);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set sensor offset calibration, error code: %d", sensor_status);
                    while (1)
                    {
                    }
                }
            }
            if (sensor_xtalk_calibration_ != -1)
            {
                ESP_LOGI(CALIBRATION, "Setting sensor xtalk calibration to %d", sensor_xtalk_calibration_);
                sensor_status = distanceSensor.SetXTalk(sensor_xtalk_calibration_);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set sensor offset calibration, error code: %d", sensor_status);
                    while (1)
                    {
                    }
                }
            }

            if (invert_direction_)
            {
                ESP_LOGD(TAG, "Inverting direction");
                LEFT = 1;
                RIGHT = 0;
            }
            if (calibration_active_)
            {
                calibration(distanceSensor);
                App.feed_wdt();
            }
            if (manual_active_)
            {
                center[0] = 167;
                center[1] = 231;
                distanceSensor.SetROI(Roode::roi_width_, Roode::roi_height_);
                setSensorMode(sensor_mode, timing_budget_);
                DIST_THRESHOLD_MAX[0] = Roode::manual_threshold_;
                DIST_THRESHOLD_MAX[1] = Roode::manual_threshold_;
                publishSensorConfiguration(DIST_THRESHOLD_MAX, true);
            }
            if (restore_values_)
            {
                ESP_LOGI("Roode setup", "Restoring last count value...");
                peopleCounter = EEPROM.read(100);
                if (peopleCounter == 255) // 255 is the default value if no value was stored
                    peopleCounter = 0;
                ESP_LOGD("Roode setup", "last value: %u", peopleCounter);
            }
            sendCounter(peopleCounter);
            distanceSensor.SetInterMeasurementInMs(delay_between_measurements);
            distanceSensor.StartRanging();
        }

        void Roode::update()
        {
            if (distance_sensor != nullptr)
            {
                distance_sensor->publish_state(distance);
            }
        }

        void Roode::loop()
        {
            // unsigned long start = micros();
            getZoneDistance();
            zone++;
            zone = zone % 2;
            App.feed_wdt();
            // unsigned long end = micros();
            // unsigned long delta = end - start;
            // ESP_LOGI("Roode loop", "loop took %lu microseconds", delta);
        }

        bool Roode::handleSensorStatus()
        {
            ESP_LOGD(TAG, "Sensor status: %d, Last sensor status: %d", sensor_status, last_sensor_status);
            bool check_status = false;
            if (last_sensor_status == sensor_status && sensor_status == VL53L1_ERROR_NONE)
            {
                if (status_sensor != nullptr)
                {
                    status_sensor->publish_state(sensor_status);
                }
                check_status = true;
            }
            if (sensor_status < 28 && sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(TAG, "Ranging failed with an error. status: %d", sensor_status);
                status_sensor->publish_state(sensor_status);
                check_status = false;
            }

            last_sensor_status = sensor_status;
            sensor_status = VL53L1_ERROR_NONE;
            return check_status;
        }

        uint16_t Roode::getDistance()
        {
            // Checking if data is available. This can also be done through the hardware interrupt. See the ReadDistanceHardwareInterrupt for an example
            uint8_t dataReady = false;
            while (!dataReady)
            {
                status += distanceSensor.CheckForDataReady(&dataReady);
                delay(1);
            }

            // Get the results
            uint16_t distance;
            sensor_status += distanceSensor.GetDistanceInMm(&distance);
            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(TAG, "Could not get distance, error code: %d", sensor_status);
                return sensor_status;
            }
            // After reading the results reset the interrupt to be able to take another measurement
            distanceSensor.ClearInterrupt();
            return distance;
        }

        void Roode::getZoneDistance()
        {
            static int PathTrack[] = {0, 0, 0, 0};
            static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
            static int LeftPreviousStatus = NOBODY;
            static int RightPreviousStatus = NOBODY;
            static uint8_t DistancesTableSize[2] = {0, 0};
            int CurrentZoneStatus = NOBODY;
            int AllZonesCurrentStatus = 0;
            int AnEventHasOccured = 0;
            sensor_status += distanceSensor.SetROICenter(center[zone]);
            sensor_status += distanceSensor.StartRanging();
            last_sensor_status = sensor_status;
            distance = getDistance();
            sensor_status += distanceSensor.StopRanging();
            if (!handleSensorStatus())
            {
                return;
            }

            if (use_sampling_)
            {
                ESP_LOGD(SETUP, "Using sampling");
                static uint16_t Distances[2][DISTANCES_ARRAY_SIZE];
                uint16_t MinDistance;
                uint8_t i;
                if (DistancesTableSize[zone] < DISTANCES_ARRAY_SIZE)
                {
                    Distances[zone][DistancesTableSize[zone]] = distance;
                    DistancesTableSize[zone]++;
                    ESP_LOGD(SETUP, "Distances[%d][DistancesTableSize[zone]] = %d", zone, Distances[zone][DistancesTableSize[zone]]);
                }
                else
                {
                    for (i = 1; i < DISTANCES_ARRAY_SIZE; i++)
                        Distances[zone][i - 1] = Distances[zone][i];
                    Distances[zone][DISTANCES_ARRAY_SIZE - 1] = distance;
                    ESP_LOGD(SETUP, "Distances[%d][DISTANCES_ARRAY_SIZE - 1] = %d", zone, Distances[zone][DISTANCES_ARRAY_SIZE - 1]);
                }
                ESP_LOGD(SETUP, "Distances[%d][0]] = %d", zone, Distances[zone][0]);
                ESP_LOGD(SETUP, "Distances[%d][1]] = %d", zone, Distances[zone][1]);
                // pick up the min distance
                MinDistance = Distances[zone][0];
                if (DistancesTableSize[zone] >= 2)
                {
                    for (i = 1; i < DistancesTableSize[zone]; i++)
                    {
                        if (Distances[zone][i] < MinDistance)
                            MinDistance = Distances[zone][i];
                    }
                }
                distance = MinDistance;
            }

            // PathTrack algorithm
            if (distance < DIST_THRESHOLD_MAX[zone] && distance > DIST_THRESHOLD_MIN[zone])
            {
                // Someone is in the sensing area
                CurrentZoneStatus = SOMEONE;
                if (presence_sensor != nullptr)
                {
                    presence_sensor->publish_state(true);
                }
            }

            // left zone
            if (zone == LEFT)
            {
                if (CurrentZoneStatus != LeftPreviousStatus)
                {
                    // event in left zone has occured
                    AnEventHasOccured = 1;

                    if (CurrentZoneStatus == SOMEONE)
                    {
                        AllZonesCurrentStatus += 1;
                    }
                    // need to check right zone as well ...
                    if (RightPreviousStatus == SOMEONE)
                    {
                        // event in right zone has occured
                        AllZonesCurrentStatus += 2;
                    }
                    // remember for next time
                    LeftPreviousStatus = CurrentZoneStatus;
                }
            }
            // right zone
            else
            {
                if (CurrentZoneStatus != RightPreviousStatus)
                {

                    // event in right zone has occured
                    AnEventHasOccured = 1;
                    if (CurrentZoneStatus == SOMEONE)
                    {
                        AllZonesCurrentStatus += 2;
                    }
                    // need to check left zone as well ...
                    if (LeftPreviousStatus == SOMEONE)
                    {
                        // event in left zone has occured
                        AllZonesCurrentStatus += 1;
                    }
                    // remember for next time
                    RightPreviousStatus = CurrentZoneStatus;
                }
            }

            // if an event has occured
            if (AnEventHasOccured)
            {
                if (PathTrackFillingSize < 4)
                {
                    PathTrackFillingSize++;
                }

                // if nobody anywhere lets check if an exit or entry has happened
                if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY))
                {

                    // check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
                    if (PathTrackFillingSize == 4)
                    {
                        // check exit or entry. no need to check PathTrack[0] == 0 , it is always the case

                        if ((PathTrack[1] == 1) && (PathTrack[2] == 3) && (PathTrack[3] == 2))
                        {
                            // This an exit
                            if (peopleCounter > 0)
                            {
                                peopleCounter--;
                                sendCounter(peopleCounter);
                                ESP_LOGD("Roode pathTracking", "Exit detected.");
                                if (entry_exit_event_sensor != nullptr)
                                {
                                    entry_exit_event_sensor->publish_state("Exit");
                                }
                                DistancesTableSize[0] = 0;
                                DistancesTableSize[1] = 0;
                            }
                        }
                        else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
                        {
                            // This an entry
                            peopleCounter++;
                            sendCounter(peopleCounter);
                            ESP_LOGD("Roode pathTracking", "Entry detected.");
                            if (entry_exit_event_sensor != nullptr)
                            {
                                entry_exit_event_sensor->publish_state("Entry");
                            }
                            DistancesTableSize[0] = 0;
                            DistancesTableSize[1] = 0;
                        }
                        else
                        {
                            // reset the table filling size also in case of unexpected path
                            DistancesTableSize[0] = 0;
                            DistancesTableSize[1] = 0;
                        }
                    }

                    PathTrackFillingSize = 1;
                }
                else
                {
                    // update PathTrack
                    // example of PathTrack update
                    // 0
                    // 0 1
                    // 0 1 3
                    // 0 1 3 1
                    // 0 1 3 3
                    // 0 1 3 2 ==> if next is 0 : check if exit
                    PathTrack[PathTrackFillingSize - 1] = AllZonesCurrentStatus;
                }
            }
            if (presence_sensor != nullptr)
            {
                if (CurrentZoneStatus == NOBODY && LeftPreviousStatus == NOBODY && RightPreviousStatus == NOBODY)
                {
                    // nobody is in the sensing area
                    presence_sensor->publish_state(false);
                }
            }
        }

        void Roode::sendCounter(uint16_t counter)
        {
            ESP_LOGI(SETUP, "Sending people count: %d", counter);
            peopleCounter = counter;
            if (people_counter_sensor != nullptr)
            {
                people_counter_sensor->publish_state(peopleCounter);
            }

            if (restore_values_)
            {
                EEPROM.write(100, peopleCounter);
                EEPROM.commit();
            }
        }
        void Roode::recalibration()
        {
            calibration(distanceSensor);
        }
        void Roode::roi_calibration(VL53L1X_ULD distanceSensor, int optimized_zone_0, int optimized_zone_1)
        {
            // the value of the average distance is used for computing the optimal size of the ROI and consequently also the center of the two zones
            int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(optimized_zone_0, optimized_zone_1) / 1000)));
            int ROI_size = min(8, max(4, function_of_the_distance));
            Roode::roi_width_ = ROI_size;
            Roode::roi_height_ = ROI_size * 2;
            // now we set the position of the center of the two zones
            if (advised_sensor_orientation_)
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
            // we will now repeat the calculations necessary to define the thresholds with the updated zones
            zone = 0;
            int *values_zone_0 = new int[number_attempts];
            int *values_zone_1 = new int[number_attempts];
            sensor_status += distanceSensor.StopRanging();
            sensor_status += distanceSensor.SetROI(Roode::roi_width_, Roode::roi_height_);
            sensor_status += distanceSensor.SetInterMeasurementInMs(delay_between_measurements);
            sensor_status += distanceSensor.StartRanging();
            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(SETUP, "Error in calibration: %d", sensor_status);
            }

            for (int i = 0; i < number_attempts; i++)
            {
                // increase sum of values in Zone 0
                distanceSensor.SetROICenter(center[zone]);
                distance = getDistance();
                values_zone_0[i] = distance;
                zone++;
                zone = zone % 2;
                App.feed_wdt();
                // increase sum of values in Zone 1
                distanceSensor.SetROICenter(center[zone]);
                distance = getDistance();
                values_zone_1[i] = distance;
                zone++;
                zone = zone % 2;
            }
            optimized_zone_0 = getOptimizedValues(values_zone_0, getSum(values_zone_0, number_attempts), number_attempts);
            optimized_zone_1 = getOptimizedValues(values_zone_1, getSum(values_zone_1, number_attempts), number_attempts);
        }
        void Roode::setSensorMode(int sensor_mode, int new_timing_budget)
        {
            distanceSensor.StopRanging();
            switch (sensor_mode)
            {
            case 0: // short mode
                time_budget_in_ms = time_budget_in_ms_short;
                delay_between_measurements = time_budget_in_ms + 5;
                sensor_status = distanceSensor.SetDistanceMode(Short);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Short);
                }
                ESP_LOGI(SETUP, "Set short mode. timing_budget: %d", time_budget_in_ms);
                break;
            case 1: // medium mode
                time_budget_in_ms = time_budget_in_ms_medium;
                delay_between_measurements = time_budget_in_ms + 5;
                sensor_status = distanceSensor.SetDistanceMode(Long);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
                }
                ESP_LOGI(SETUP, "Set medium mode. timing_budget: %d", time_budget_in_ms);
                break;
            case 2: // long mode
                time_budget_in_ms = time_budget_in_ms_long;
                delay_between_measurements = time_budget_in_ms + 5;
                sensor_status = distanceSensor.SetDistanceMode(Long);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
                }
                ESP_LOGI(SETUP, "Set long range mode. timing_budget: %d", time_budget_in_ms);
                break;
            case 3: // custom mode
                time_budget_in_ms = new_timing_budget;
                delay_between_measurements = new_timing_budget + 5;
                sensor_status = distanceSensor.SetDistanceMode(Long);
                if (sensor_status != VL53L1_ERROR_NONE)
                {
                    ESP_LOGE(SETUP, "Could not set distance mode.  mode: %d", Long);
                }
                ESP_LOGI(SETUP, "Manually set custom range mode. timing_budget: %d", time_budget_in_ms);
                break;
            default:
                break;
            }
            sensor_status = distanceSensor.SetTimingBudgetInMs(time_budget_in_ms);
            if (sensor_status != 0)
            {
                ESP_LOGE(SETUP, "Could not set timing budget.  timing_budget: %d ms", time_budget_in_ms);
            }
        }

        void Roode::setCorrectDistanceSettings(float average_zone_0, float average_zone_1)
        {
            if (average_zone_0 <= short_distance_threshold || average_zone_1 <= short_distance_threshold)
            {
                setSensorMode(0, time_budget_in_ms_short);
            }

            if ((average_zone_0 > short_distance_threshold && average_zone_0 <= medium_distance_threshold) || (average_zone_1 > short_distance_threshold && average_zone_1 <= medium_distance_threshold))
            {
                setSensorMode(1, time_budget_in_ms_medium);
            }

            if (average_zone_0 > medium_distance_threshold || average_zone_1 > medium_distance_threshold)
            {
                setSensorMode(2, time_budget_in_ms_long);
            }
            sensor_status = distanceSensor.SetTimingBudgetInMs(time_budget_in_ms);
            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(CALIBRATION, "Could not set timing budget. timing_budget: %d ms, status: %d", time_budget_in_ms, sensor_status);
            }
        }
        int Roode::getSum(int *array, int size)
        {
            int sum = 0;
            for (int i = 0; i < size; i++)
            {
                sum = sum + array[i];
                App.feed_wdt();
            }
            return sum;
        }
        int Roode::getOptimizedValues(int *values, int sum, int size)
        {
            int sum_squared = 0;
            int variance = 0;
            int sd = 0;
            int avg = sum / size;

            for (int i = 0; i < size; i++)
            {
                sum_squared = sum_squared + (values[i] * values[i]);
                App.feed_wdt();
            }
            variance = sum_squared / size - (avg * avg);
            sd = sqrt(variance);
            ESP_LOGD(CALIBRATION, "Zone AVG: %d", avg);
            ESP_LOGD(CALIBRATION, "Zone 0 SD: %d", sd);
            return avg - sd;
        }

        void Roode::calibration(VL53L1X_ULD distanceSensor)
        {
            distanceSensor.StopRanging();
            // the sensor does 100 measurements for each zone (zones are predefined)
            time_budget_in_ms = time_budget_in_ms_medium;
            delay_between_measurements = time_budget_in_ms + 5;
            distanceSensor.SetDistanceMode(Long);
            sensor_status = distanceSensor.SetTimingBudgetInMs(time_budget_in_ms);

            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(CALIBRATION, "Could not set timing budget. timing_budget: %d ms, status: %d", time_budget_in_ms, sensor_status);
            }
            if (advised_sensor_orientation_)
            {
                center[0] = 167;
                center[1] = 231;
            }
            else
            {
                center[0] = 195;
                center[1] = 60;
                uint16_t roi_width_temp = roi_width_;
                uint16_t roi_height_temp = roi_height_;
                roi_width_ = roi_height_;
                roi_height_ = roi_width_;
            }

            zone = 0;

            int *values_zone_0 = new int[number_attempts];
            int *values_zone_1 = new int[number_attempts];
            distanceSensor.SetROI(Roode::roi_width_, Roode::roi_height_);
            distanceSensor.SetInterMeasurementInMs(delay_between_measurements);
            distanceSensor.StartRanging();
            for (int i = 0; i < number_attempts; i++)
            {
                // increase sum of values in Zone 0
                distanceSensor.SetROICenter(center[zone]);
                distance = getDistance();
                values_zone_0[i] = distance;
                zone++;
                zone = zone % 2;
                App.feed_wdt();
                // increase sum of values in Zone 1
                distanceSensor.SetROICenter(center[zone]);
                distance = getDistance();
                values_zone_1[i] = distance;
                zone++;
                zone = zone % 2;
            }

            // after we have computed the sum for each zone, we can compute the average distance of each zone

            optimized_zone_0 = getOptimizedValues(values_zone_0, getSum(values_zone_0, number_attempts), number_attempts);
            optimized_zone_1 = getOptimizedValues(values_zone_1, getSum(values_zone_1, number_attempts), number_attempts);
            setCorrectDistanceSettings(optimized_zone_0, optimized_zone_1);
            if (roi_calibration_)
            {
                roi_calibration(distanceSensor, optimized_zone_0, optimized_zone_1);
            }

            DIST_THRESHOLD_MAX[0] = optimized_zone_0 * max_threshold_percentage_ / 100; // they can be int values, as we are not interested in the decimal part when defining the threshold
            DIST_THRESHOLD_MAX[1] = optimized_zone_1 * max_threshold_percentage_ / 100;
            int hundred_threshold_zone_0 = DIST_THRESHOLD_MAX[0] / 100;
            int hundred_threshold_zone_1 = DIST_THRESHOLD_MAX[1] / 100;
            int unit_threshold_zone_0 = DIST_THRESHOLD_MAX[0] - 100 * hundred_threshold_zone_0;
            int unit_threshold_zone_1 = DIST_THRESHOLD_MAX[1] - 100 * hundred_threshold_zone_1;
            publishSensorConfiguration(DIST_THRESHOLD_MAX, true);
            App.feed_wdt();
            if (min_threshold_percentage_ != 0)
            {
                DIST_THRESHOLD_MIN[0] = optimized_zone_0 * min_threshold_percentage_ / 100; // they can be int values, as we are not interested in the decimal part when defining the threshold
                DIST_THRESHOLD_MIN[1] = optimized_zone_1 * min_threshold_percentage_ / 100;
                publishSensorConfiguration(DIST_THRESHOLD_MIN, false);
            }
            distanceSensor.StopRanging();
        }

        void Roode::publishSensorConfiguration(int DIST_THRESHOLD_ARR[2], bool isMax)
        {
            if (isMax)
            {
                ESP_LOGI(SETUP, "Max threshold zone0: %dmm", DIST_THRESHOLD_ARR[0]);
                ESP_LOGI(SETUP, "Max threshold zone1: %dmm", DIST_THRESHOLD_ARR[1]);
                if (max_threshold_zone0_sensor != nullptr)
                {
                    max_threshold_zone0_sensor->publish_state(DIST_THRESHOLD_ARR[0]);
                }

                if (max_threshold_zone1_sensor != nullptr)
                {
                    max_threshold_zone1_sensor->publish_state(DIST_THRESHOLD_ARR[1]);
                }
            }
            else
            {
                ESP_LOGI(SETUP, "Min threshold zone0: %dmm", DIST_THRESHOLD_ARR[0]);
                ESP_LOGI(SETUP, "Min threshold zone1: %dmm", DIST_THRESHOLD_ARR[1]);
                if (min_threshold_zone0_sensor != nullptr)
                {
                    min_threshold_zone0_sensor->publish_state(DIST_THRESHOLD_ARR[0]);
                }
                if (min_threshold_zone1_sensor != nullptr)
                {
                    min_threshold_zone1_sensor->publish_state(DIST_THRESHOLD_ARR[1]);
                }
            }

            if (roi_height_sensor != nullptr)
            {
                roi_height_sensor->publish_state(roi_height_);
            }
            if (roi_width_sensor != nullptr)
            {
                roi_width_sensor->publish_state(roi_width_);
            }
        }
    }
}