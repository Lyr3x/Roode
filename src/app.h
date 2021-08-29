#include "esphome.h"

#include <Wire.h>
#include <Config.h>
#include <VL53L1X.h>
#include <Counter.h>
#include <EEPROM.h>
// #include <Calibration.h>

#define USE_VL53L1X
VL53L1X distanceSensor;
#define NOBODY 0
#define SOMEONE 1
#define LEFT 0
#define RIGHT 1

static const char *TAG = "main";
int distance = 0;
int left = 0, right = 0, oldcnt;
static uint8_t peopleCount = 0; //default state: nobody is inside the room
boolean lastTrippedState = 0;

//static int num_timeouts = 0;
double people, distance_avg;

//Remove
static int DIST_THRESHOLD_MAX[] = {0, 0}; // treshold of the two zones
static int MIN_DISTANCE[] = {0, 0};
static int center[2] = {0, 0}; /* center of the two zones */
static int ROI_height = 0;
static int ROI_width = 0;
static int zone = 0;

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
    // This will be called by App.setup()
    Wire.begin();
    Wire.setClock(400000);

    distanceSensor.setTimeout(500);
    if (!distanceSensor.init())
    {
      ESP_LOGI("VL53L1X custom sensor", "Failed to detect and initialize sensor!");
      while (1);
    }
#ifdef CALIBRATION
    calibration(countSensor);
#endif
#ifdef CALIBRATIONV2
    // calibration_boot(countSensor);
#endif
    ESP_LOGI("VL53L1X custom sensor", "Starting measurements");
    distanceSensor.setDistanceMode(VL53L1X::Long);
    distanceSensor.setMeasurementTimingBudget(50000);
    distanceSensor.startContinuous(50);
  }

  void checkMQTTCommands()
  {
    if (resetCounter == 1)
    {
      ESP_LOGI("MQTTCommand", "Reset counter command received");
      resetCounter = 0;
      sendCounter(-1);
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
      publishMQTT(id(cnt));
      forceSetValue = -1;
    }
  }

  void publishMQTT(int val)
  {
    peopleCount = val;
    people_sensor->publish_state(val);
  }
  void loop() override
  {
    checkMQTTCommands();
    static int PathTrack[] = {0, 0, 0, 0};
    static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
    static int LeftPreviousStatus = NOBODY;
    static int RightPreviousStatus = NOBODY;

    int CurrentZoneStatus = NOBODY;
    int AllZonesCurrentStatus = 0;
    int AnEventHasOccured = 0;
    if (zone == 0)
    {
      distance = distanceSensor.read();
    }
    else if (zone == 1)
    {
      distance = distanceSensor.read();
    }

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
            //PeopleCount --;
            if (id(cnt) > 0)
              id(cnt)--;
            right = 1;
            dispUpdate();
            right = 0;
          }
          else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
          {
            // This an entry
            //PeopleCount ++;
            id(cnt)++;
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

    zone++;
    zone = zone % 2;

    if (resetCounter == 1)
    {
      resetCounter = 0;
      sendCounter(-1);
    }
  }
  inline void dispUpdate()
  { // 33mS
    // left = in
    if (left)
    {
      // Serial.println("--->");
      ESP_LOGD("VL53L1X custom sensor", "--->");
      sendCounter(1);
    }
    // right = out
    if (right)
    {
      // Serial.println("<---");
      ESP_LOGD("VL53L1X custom sensor", "<---");
      sendCounter(0);
    }
    Serial.println(id(cnt));
    ESP_LOGD("VL53L1X custom sensor", "Count: %d", id(cnt));
  }
  void sendCounter(int inout)
  {
    if (inout == 1)
    {
      peopleCount++;
    }
    else if (inout == 0)
    {
      if (peopleCount > 0)
      {
        peopleCount--;
      }
    }
    else if (inout == -1)
    {
      peopleCount = 0;
    }

    ESP_LOGI("VL53L1X custom sensor", "Sending people count: %d", peopleCount);
    people_sensor->publish_state(peopleCount);
  }
};
