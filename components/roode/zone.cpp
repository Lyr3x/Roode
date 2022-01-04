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
        }

        uint16_t Zone::readDistance(VL53L1X_ULD &distanceSensor)
        {
            last_sensor_status = sensor_status;

            sensor_status += distanceSensor.StartRanging();
            sensor_status += distanceSensor.SetROICenter(this->getRoiCenter());
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
            return distance;
        }
        uint16_t Zone::getDistance()
        {
            return this->distance;
        }
        uint16_t Zone::getRoiWidth()
        {
            return this->roi_width;
        }
        uint16_t Zone::getRoiHeight()
        {
            return this->roi_height;
        }
        uint16_t Zone::getRoiCenter()
        {
            return this->roi_center;
        }
        uint16_t Zone::setRoiWidth(uint16_t new_roi_width)
        {
            return this->roi_width = new_roi_width;
        }
        uint16_t Zone::setRoiHeight(uint16_t new_roi_height)
        {
            return this->roi_height = new_roi_height;
        }
        uint16_t Zone::setRoiCenter(uint16_t new_roi_center)
        {
            return this->roi_center = new_roi_center;
        }
        uint8_t Zone::getZoneId()
        {
            return this->id;
        }
    }
}