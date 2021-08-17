#include "esphome.h"

#include <Wire.h>
#include <Config.h>
#include <VL53L1XSensor.h>
#include <Counter.h>
#include <Calibration.h>

#define USE_VL53L1X
VL53L1XSensor count_sensor(XSHUT_PIN, SENSOR_I2C);
// VL53L1_count_sensor_t    VL53L1_sensor;
// VL53L1_count_sensor      count_sensor = &VL53L1_sensor;

class PeopleCountSensor : public Component, public Sensor
{
public:
  // constructor
  Sensor *people_sensor = new Sensor();
  Sensor *distance_sensor = new Sensor();

  void setup() override
  {
    // This will be called by App.setup()
    Wire.begin();
    Wire.setClock(400000);
    
    count_sensor.init();
    calibration(count_sensor);

    ESP_LOGI("VL53L1X custom sensor", "Starting measurements");
    count_sensor.startMeasurement();
  }

  void loop() override
  {
    counting(count_sensor, *people_sensor, *distance_sensor);
  }
};
