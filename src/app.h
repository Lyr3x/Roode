#include "esphome.h"
#include <Wire.h>
#include <Config.h>
#include "SparkFun_VL53L1X.h"
#include <Counter.h>
#include <EEPROM.h>
#include <Calibration.h>

SFEVL53L1X countSensor(Wire);
#define NOBODY 0
#define SOMEONE 1
#ifdef INVERT_DIRECTION
#define LEFT 1
#define RIGHT 0
#else
#define LEFT 1
#define RIGHT 0
#endif

static const char *TAG = "main";
int distance = 0;
int left = 0, right = 0, oldcnt;
static uint8_t peopleCount = 0; //default state: nobody is inside the room
boolean lastTrippedState = 0;
static int PathTrack[] = {0, 0, 0, 0};
static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;
static int zone = 0;

// MQTT Commands
static int resetCounter = 0;
static int forceSetValue = -1;

//static int num_timeouts = 0;
double people, distance_avg;

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

    if (countSensor.init() == false)
      Serial.println("Sensor online!");
    countSensor.setIntermeasurementPeriod(time_budget_in_ms_long);
    countSensor.setDistanceModeLong();
#ifdef CALIBRATION
    calibration(countSensor);
#endif
#ifdef CALIBRATIONV2
    calibration_boot(countSensor);
#endif
  }

  void publishMQTT(int val)
  {
    peopleCount = val;
    people_sensor->publish_state(val);
  }
  void loop() override
  {

    getNewDistanceForZone();      // get the distance for the zone
    getDirection(distance, Zone); // get the direction of the path

    Zone++;
    Zone = Zone % 2;

    checkMQTTCommands();
  }
  void checkMQTTCommands()
  {
    if (resetCounter == 1)
    {
      ESP_LOGD("MQTTCommand", "Reset counter command received");
      resetCounter = 0;
      sendCounter(-1);
    }
    if (id(recalibrate) == 1)
    {
      ESP_LOGD("MQTTCommand", "Recalibration command received");
      calibration(countSensor);
      recalibrate = 0;
    }
    if (forceSetValue != -1)
    {
      ESP_LOGD("MQTTCommand", "Force set value command received");
      publishMQTT(id(cnt));
      forceSetValue = -1;
    }
  }
  void getNewDistanceForZone()
  {
    countSensor.setROI(ROI_height, ROI_width, center[Zone]);

    if (DIST_THRESHOLD_MAX[0] < 1200 && DIST_THRESHOLD_MAX[1] < 1200)
    {
      countSensor.setDistanceModeShort();
      countSensor.setTimingBudgetInMs(time_budget_in_ms_short);
      delay_between_measurements = delay_between_measurements_short;
    }
    else
    {
      countSensor.setDistanceModeLong();
      countSensor.setTimingBudgetInMs(time_budget_in_ms_long);
      delay_between_measurements = delay_between_measurements_long;
    }
    delay(delay_between_measurements);
    countSensor.startRanging();
    distance = countSensor.getDistance();
    countSensor.stopRanging();
  }
  void getDirection(int16_t Distance, uint8_t zone)
  {

    int CurrentZoneStatus = NOBODY;
    int AllZonesCurrentStatus = 0;
    int AnEventHasOccured = 0;

    if (Distance < DIST_THRESHOLD_MAX[Zone] && Distance > MIN_DISTANCE[Zone])
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

        // event in left zone has occured
        AnEventHasOccured = 1;
        if (CurrentZoneStatus == SOMEONE)
        {
          AllZonesCurrentStatus += 2;
        }
        // need to left zone as well ...
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
          Serial.println();
          if ((PathTrack[1] == 1) && (PathTrack[2] == 3) && (PathTrack[3] == 2))
          {
            if (cnt > 0)
              cnt--;
            right = 1;
            dispUpdate();
            right = 0;
          }
          else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
          {
            cnt++;
            left = 1;
            dispUpdate();
            left = 0;
          }
        }
        for (int i = 0; i < 4; i++)
        {
          PathTrack[i] = 0;
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
