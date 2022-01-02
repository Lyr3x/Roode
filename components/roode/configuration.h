
#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/application.h"
#include "EEPROM.h"
#include "VL53L1X_ULD.h"
#include <math.h>
namespace esphome
{
    namespace roode
    {
        class Configuration
        {
        public:
            // Calibration(VL53L1X_ULD distanceSensor);
            void setSensorMode(VL53L1X_ULD distanceSensor, int sensor_mode, int timing_budget = 0);

        protected:
            VL53L1X_ULD distanceSensor;
            VL53L1_Error sensor_status = VL53L1_ERROR_NONE;
        };
    }
}
