#include "esphome/core/log.h"
#include "roode.h"
namespace esphome
{
    namespace roode
    {
        void Roode::dump_config()
        {
            LOG_SENSOR("", "ROODE", this);
            LOG_I2C_DEVICE(this);
        }

        void Roode::setup()
        {
            Wire.begin();
            Wire.setClock(400000);
            if (Roode::invert_direction_ == true)
            {
                LEFT = 1;
                RIGHT = 0;
            }
            else
            {
                LEFT = 0;
                RIGHT = 1;
            }
            // This will be called by App.setup()
            // Wire.begin();
            // Wire.setClock(400000);
            // if (id(SENSOR_I2C) != 0)
            // {
            //     ESP_LOGI("VL53L1X custom sensor", "Setting custom I2C address");
            //     distanceSensor.setAddress(id(SENSOR_I2C));
            // }

            distanceSensor.setTimeout(500);
            if (!distanceSensor.init())
            {
                ESP_LOGI("Roode setup", "Failed to detect and initialize sensor!");
                while (1)
                    ;
            }
            if (calibration_)
            {
                calibration_boot(distanceSensor);
            }
            else
            {
                DIST_THRESHOLD_MAX[0] = 800;
                DIST_THRESHOLD_MAX[1] = 800;
            }
            // distanceSensor.setROISize(roi_width_, roi_height_); //Make dynamic ROI configurable otherwise set to 7,16
        }

        void Roode::loop()
        {
            checkMQTTCommands();
            getZoneDistance();
            zone++;
            zone = zone % 2;
        }

        void Roode::checkMQTTCommands()
        {
            if (resetCounter == 1)
            {
                ESP_LOGI("MQTTCommand", "Reset counter command received");
                resetCounter = 0;
                peopleCounter = 0;
                sendCounter();
            }
            if (recalibrate == 1)
            {
                ESP_LOGI("MQTTCommand", "Recalibration command received");
                // calibration(sensor);
                recalibrate = 0;
            }
            if (forceSetValue != -1)
            {
                ESP_LOGI("MQTTCommand", "Force set value command received");
                publishMQTT(peopleCounter);
                forceSetValue = -1;
            }
        }

        void Roode::publishMQTT(int val)
        {
            peopleCounter = val;
            this->publish_state(val);
        }

        void Roode::getZoneDistance()
        {
            static int PathTrack[] = {0, 0, 0, 0};
            static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
            static int LeftPreviousStatus = NOBODY;
            static int RightPreviousStatus = NOBODY;

            int CurrentZoneStatus = NOBODY;
            int AllZonesCurrentStatus = 0;
            int AnEventHasOccured = 0;

            distanceSensor.setROICenter(center[zone]);
            distanceSensor.startContinuous(delay_between_measurements);
            distance = distanceSensor.read();
            distanceSensor.stopContinuous();
            if (distance < DIST_THRESHOLD_MAX[zone] && distance > MIN_DISTANCE[zone])
            {
                // Someone is in !
                CurrentZoneStatus = SOMEONE;
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
                            //peopleCounter --;
                            if (peopleCounter > 0)
                            {
                                peopleCounter--;
                                sendCounter();
                                ESP_LOGD("Roode pathTracking", "Exit detected.");
                            }

                            right = 1;
                            right = 0;
                        }
                        else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
                        {
                            // This an entry
                            //peopleCounter ++;
                            peopleCounter++;
                            sendCounter();
                            ESP_LOGD("Roode pathTracking", "Entry detected.");
                            left = 1;
                            left = 0;
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
        }

        void Roode::sendCounter()
        {
            ESP_LOGI("VL53L1X custom sensor", "Sending people count: %d", peopleCounter);
            this->publish_state(Roode::peopleCounter);
        }

        void Roode::roi_calibration(VL53L1X distanceSensor)
        {
            // the value of the average distance is used for computing the optimal size of the ROI and consequently also the center of the two zones
            int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(average_zone_0, average_zone_1) / 1000)));
            delay(1000);
            int ROI_size = min(8, max(4, function_of_the_distance));
            Roode::roi_width_ = ROI_size;
            Roode::roi_height_ = ROI_size;
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
                distanceSensor.setROISize(Roode::roi_width_, Roode::roi_height_);
                distanceSensor.setROICenter(center[zone]);
                distanceSensor.startContinuous(delay_between_measurements);
                distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
                distance = distanceSensor.read();
                distanceSensor.stopContinuous();
                sum_zone_0 = sum_zone_0 + distance;
                zone++;
                zone = zone % 2;

                // increase sum of values in Zone 1
                distanceSensor.setROISize(Roode::roi_width_, Roode::roi_height_);
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

        void Roode::calibration(VL53L1X distanceSensor)
        {
            // the sensor does 100 measurements for each zone (zones are predefined)
            time_budget_in_ms = time_budget_in_ms_long;
            delay_between_measurements = delay_between_measurements_long;
            distanceSensor.startContinuous(delay_between_measurements);
            distanceSensor.setDistanceMode(VL53L1X::Long);
            distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
            if (advised_orientation_of_the_sensor)
            {
                center[0] = 167;
                center[1] = 231;
            }
            else
            {
                center[0] = 195;
                center[1] = 60;
            }

            delay(500);

            zone = 0;

            for (int i = 0; i < number_attempts; i++)
            {
                // increase sum of values in Zone 1
                distanceSensor.setROISize(Roode::roi_width_, Roode::roi_height_);
                distanceSensor.setROICenter(center[zone]);
                distanceSensor.startContinuous(delay_between_measurements);
                distanceSensor.setMeasurementTimingBudget(time_budget_in_ms * 1000);
                distance = distanceSensor.read();
                distanceSensor.stopContinuous();
                sum_zone_0 = sum_zone_0 + distance;
                zone++;
                zone = zone % 2;

                // increase sum of values in Zone 2
                distanceSensor.setROISize(Roode::roi_width_, Roode::roi_height_);
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

            if (Roode::roi_calibration_)
            {
                roi_calibration(distanceSensor);
            }

            float threshold_zone_0 = average_zone_0 * Roode::threshold_percentage_ / 100; // they can be int values, as we are not interested in the decimal part when defining the threshold
            float threshold_zone_1 = average_zone_1 * Roode::threshold_percentage_ / 100;

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

        void Roode::calibration_boot(VL53L1X distanceSensor)
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
                    Roode::roi_height_ = EEPROM.read(3);
                    Roode::roi_width_ = EEPROM.read(3);
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

        class I2CComponentDummy : public i2c::I2CComponent
        {
        public:
            TwoWire *get_wire() const { return this->wire_; }
        };

        // sets
        void Roode::set_i2c_parent(i2c::I2CComponent *parent)
        {
            distanceSensor.setBus(static_cast<I2CComponentDummy *>(parent)->get_wire());
        }

        void Roode::set_i2c_address(uint8_t address)
        {
            if (distanceSensor.getAddress() != address)
            {
                distanceSensor.setAddress(address);
            }
        }
    }
}