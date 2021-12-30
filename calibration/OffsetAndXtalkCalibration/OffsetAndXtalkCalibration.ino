#include "VL53L1X_ULD.h"

VL53L1X_ULD sensor;
bool dataReady;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    VL53L1_Error sensor_status = sensor.Begin();
    if (sensor_status != VL53L1_ERROR_NONE)
    {
        Serial.println("Could not initialize the sensor, error code: " + String(sensor_status));
        while (1)
        {
        }
    }
    Serial.println("Sensor initialized");

    // Start the offset calibration
    Serial.println("Place a target, 17 % grey, at 140 mm from the sensor a");
    Serial.println("The calibration may take a few seconds. The offset correction is applied to the sensor at the end of calibration.");
    Serial.readString();
    int16_t foundOffset;
    sensor_status = sensor.CalibrateOffset(140, &foundOffset);


    Serial.println("Calibrated offset: " + String(foundOffset));
    Serial.println("Set this offset in the sensor configuration under offset: <value>");

    /* The target distance : the distance where the sensor start to "under range"
    Crosstalk calibration should be conducted in a dark environment, with no IR contribution.
    The crosstalk calibration distance needs to be characterized as it depends on the system environment which
    mainly includes:
    • The cover glass material and optical properties
    • The air gap value i.e. the distance between the sensor and the cover glass
    Do a full sweep with the target from near to far, noting the resulting measurement.
    At some point, the actual value and the measured value start to diverge. This is the crosstalk calibration
    distance.
    */
    uint16_t CalibrationDistance = 140; // crosstalk calibration distance
    uint16_t foundXTalk;
    sensor_status = sensor.CalibrateXTalk(CalibrationDistance, &foundXTalk);
    Serial.println("Calibrated offset: " + String(foundXTalk));
    Serial.println("Set this offset in the sensor configuration under crosstalk: <value>");
}
void loop()
{
    if (dataReady)
    {
        // Get the results
        uint16_t distance;
        sensor.GetDistanceInMm(&distance);

        // After reading the results reset the interrupt to be able to take another measurement
        sensor.ClearInterrupt();
        dataReady = false;

        Serial.println("Distance in mm: " + String(distance));
    }
}
