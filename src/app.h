#include "esphome.h"

#include <Wire.h>
#include <EEPROM.h>
#include <Calibration.h>

#define USE_VL53L1X
VL53L1X distanceSensor;
#define NOBODY 0
#define SOMEONE 1
static int LEFT = 0;
static int RIGHT = 1;

static const char *TAG = "main";
int distance = 0;
int left = 0, right = 0, oldcnt;
boolean lastTrippedState = 0;
//static int num_timeouts = 0;
double people, distance_avg;

// MQTT Commands
static int resetCounter = 0;
static int forceSetValue = -1;

class PeopleCountSensor : public Component, public Sensor
{
public:
  // constructor
  Sensor *people_sensor = new Sensor();
  Sensor *distance_sensor = new Sensor();

  void setup() override
  {
    if (id(INVERT_DIRECTION) == true)
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
    Wire.begin();
    Wire.setClock(400000);
    if (id(SENSOR_I2C) != 0)
    {
      ESP_LOGI("VL53L1X custom sensor", "Setting custom I2C address");
      distanceSensor.setAddress(id(SENSOR_I2C));
    }

    distanceSensor.setTimeout(500);
    if (!distanceSensor.init())
    {
      ESP_LOGI("VL53L1X custom sensor", "Failed to detect and initialize sensor!");
      while (1)
        ;
    }
    if (id(CALIBRATION))
    {
      calibration_boot(distanceSensor);
    }
    else
    {
      DIST_THRESHOLD_MAX[0] = 800;
      DIST_THRESHOLD_MAX[1] = 800;
    }
  
  }

  void checkMQTTCommands()
  {
    if (resetCounter == 1)
    {
      ESP_LOGI("MQTTCommand", "Reset counter command received");
      resetCounter = 0;
      id(peopleCounter) = 0;
      sendCounter();
    }
    if (id(recalibrate) == 1)
    {
      ESP_LOGI("MQTTCommand", "Recalibration command received");
      // calibration(sensor);
      recalibrate = 0;
    }
    if (forceSetValue != -1)
    {
      ESP_LOGI("MQTTCommand", "Force set value command received");
      publishMQTT(id(peopleCounter));
      forceSetValue = -1;
    }
  }

  void publishMQTT(int val)
  {
    id(peopleCounter) = val;
    people_sensor->publish_state(val);
  }

  void getZoneDistance()
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
            if (id(peopleCounter) > 0)
              id(peopleCounter)--;
            right = 1;
            dispUpdate();
            right = 0;
          }
          else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
          {
            // This an entry
            //peopleCounter ++;
            id(peopleCounter)++;
            left = 1;
            dispUpdate();
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
  void loop() override
  {
    checkMQTTCommands();
    getZoneDistance();
    zone++;
    zone = zone % 2;
  }
  inline void dispUpdate()
  { // 33mS
    // left = in
    if (left)
    {
      // Serial.println("--->");
      ESP_LOGD("VL53L1X custom sensor", "--->");
      sendCounter();
    }
    // right = out
    if (right)
    {
      // Serial.println("<---");
      ESP_LOGD("VL53L1X custom sensor", "<---");
      sendCounter();
    }
    Serial.println(id(peopleCounter));
    ESP_LOGD("VL53L1X custom sensor", "Count: %d", id(peopleCounter));
  }
  void sendCounter()
  {
    ESP_LOGI("VL53L1X custom sensor", "Sending people count: %d", id(peopleCounter));
    people_sensor->publish_state(id(peopleCounter));
  }
};
