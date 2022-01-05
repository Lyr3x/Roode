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

        uint16_t Zone::readDistance(VL53L1X_ULD &distanceSensor)
        {
            last_sensor_status = sensor_status;
            sensor_status += distanceSensor.SetROI(this->getRoiWidth(), this->getRoiHeight());
            sensor_status += distanceSensor.SetROICenter(this->getRoiCenter());
            sensor_status += distanceSensor.StartRanging();
            // Checking if data is available. This can also be done through the hardware interrupt. See the ReadDistanceHardwareInterrupt for an example
            uint8_t dataReady = false;
            while (!dataReady)
            {
                sensor_status += distanceSensor.CheckForDataReady(&dataReady);
                delay(1);
            }

            // Get the results
            sensor_status += distanceSensor.GetDistanceInMm(&distance);

            if (sensor_status != VL53L1_ERROR_NONE)
            {
                ESP_LOGE(TAG, "Could not get distance, error code: %d", sensor_status);
                return sensor_status;
            }
            // After reading the results reset the interrupt to be able to take another measurement
            sensor_status += distanceSensor.ClearInterrupt();
            sensor_status += distanceSensor.StopRanging();
            return distance;
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
        void Zone::updateRoi(uint16_t new_width, uint16_t new_height, uint16_t new_center)
        {
            this->roi.width = new_width;
            this->roi.height = new_height;
            this->roi.center = new_center;
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