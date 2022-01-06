#include "zone.h"

static uint8_t zone_id = 0;
namespace esphome
{
    namespace roode
    {
        Zone::Zone(int roi_width, int roi_height, int roi_center)
        {
            this->id = zone_id++;
            ESP_LOGD(TAG, "Zone(%d, %d, %d)", roi_width, roi_height, roi_center);
            this->roi_width = roi_width;
            this->roi_height = roi_height;
            this->roi_center = roi_center;
            this->roi = {roi_width, roi_height, roi_center};
        }

        VL53L1_Error Zone::readDistance(VL53L1X_ULD &distanceSensor)
        {
            last_sensor_status = sensor_status;
            sensor_status += distanceSensor.SetROI(this->getRoiWidth(), this->getRoiHeight());
            sensor_status += distanceSensor.SetROICenter(this->getRoiCenter());
            sensor_status += distanceSensor.StartRanging();

            // Wait for the measurement to be ready
            uint8_t dataReady = false;
            while (!dataReady)
            {
                sensor_status += distanceSensor.CheckForDataReady(&dataReady);
                delay(1);
            }

            // Get the results
            sensor_status += distanceSensor.GetDistanceInMm(&distance);

            // After reading the results reset the interrupt to be able to take another measurement
            sensor_status += distanceSensor.ClearInterrupt();
            sensor_status += distanceSensor.StopRanging();
            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(TAG, "Could not get distance, error code: %d", sensor_status);
                return sensor_status;
            }
            return sensor_status;
        }

        int Zone::calibrateThreshold(VL53L1X_ULD &distanceSensor, int number_attempts, uint16_t max_threshold_percentage, uint16_t min_threshold_percentage)
        {
            int *zone_distances = new int[number_attempts];
            int sum = 0;
            for (int i = 0; i < number_attempts; i++)
            {
                this->readDistance(distanceSensor);
                zone_distances[i] = this->getDistance();
                sum += zone_distances[i];
            };
            int optimized_threshold = this->getOptimizedValues(zone_distances, sum, number_attempts);
            this->setMaxThreshold(optimized_threshold * max_threshold_percentage / 100);
            if (min_threshold_percentage > 0)
                this->setMinThreshold(optimized_threshold * min_threshold_percentage / 100);
            ESP_LOGI(CALIBRATION, "Calibrated threshold for zone. zoneId: %d, threshold: %d", this->getZoneId(), optimized_threshold);
            return optimized_threshold;
        }
        void Zone::roi_calibration(VL53L1X_ULD &distanceSensor, int entry_threshold, int exit_threshold, bool sensor_orientation)
        {
            // the value of the average distance is used for computing the optimal size of the ROI and consequently also the center of the two zones
            int function_of_the_distance = 16 * (1 - (0.15 * 2) / (0.34 * (min(entry_threshold, exit_threshold) / 1000)));
            int ROI_size = min(8, max(4, function_of_the_distance));
            this->updateRoi(ROI_size, ROI_size * 2);
            // now we set the position of the center of the two zones
            if (sensor_orientation)
            {
                switch (ROI_size)
                {
                case 4:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(150);
                    this->setRoiCenter(247);

                    break;
                case 5:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(159);
                    this->setRoiCenter(239);
                    break;
                case 6:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(159);
                    this->setRoiCenter(239);
                    break;
                case 7:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(167);
                    this->setRoiCenter(231);
                    break;
                case 8:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(167);
                    this->setRoiCenter(231);
                    break;
                }
            }
            else
            {
                switch (ROI_size)
                {
                case 4:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(193);
                    this->setRoiCenter(58);
                    break;
                case 5:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(194);
                    this->setRoiCenter(59);
                    break;
                case 6:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(194);
                    this->setRoiCenter(59);
                    break;
                case 7:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(195);
                    this->setRoiCenter(60);
                    break;
                case 8:
                    if (this->getZoneId() == 0)
                        this->setRoiCenter(195);
                    this->setRoiCenter(60);
                    break;
                }
            }
            ESP_LOGI(CALIBRATION, "Calibrated ROI for zone. zoneId: %d, width: %d, height: %d, center: %d", this->getZoneId(), this->getRoiWidth(), this->getRoiHeight(), this->getRoiCenter());
        }
        int Zone::getOptimizedValues(int *values, int sum, int size)
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
            ESP_LOGD(CALIBRATION, "Zone SD: %d", sd);
            return avg - sd;
        }

        uint16_t Zone::getDistance()
        {
            return this->distance;
        }
        uint16_t Zone::getRoiWidth()
        {
            return this->roi.width;
        }
        uint16_t Zone::getRoiHeight()
        {
            return this->roi.height;
        }
        uint16_t Zone::getRoiCenter()
        {
            return this->roi.center;
        }
        void Zone::setRoiWidth(uint16_t new_roi_width)
        {
            this->roi.width = new_roi_width;
        }
        void Zone::setRoiHeight(uint16_t new_roi_height)
        {
            this->roi.height = new_roi_height;
        }
        void Zone::setRoiCenter(uint16_t new_roi_center)
        {
            this->roi.center = new_roi_center;
        }
        void Zone::updateRoi(uint16_t new_width, uint16_t new_height)
        {
            this->roi.width = new_width;
            this->roi.height = new_height;
        }
        void Zone::setMinThreshold(uint16_t min)
        {
            this->threshold.min = min;
        }
        void Zone::setMaxThreshold(uint16_t max)
        {
            this->threshold.max = max;
        }
        uint16_t Zone::getMinThreshold()
        {
            return this->threshold.min;
        }
        uint16_t Zone::getMaxThreshold()
        {
            return this->threshold.max;
        }

        uint8_t Zone::getZoneId()
        {
            return this->id;
        }
    }
}