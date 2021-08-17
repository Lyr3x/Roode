#ifndef COUNTER_H
#define COUNTER_H
#pragma once

#include <Config.h>
static VL53L1_RangingMeasurementData_t RangingData;
int gesture_code;
#define NOBODY 0
#define SOMEONE 1
#define LEFT 0
#define RIGHT 1

static const char *TAG = "main";

int VL53L1_status, i;
int distance = 0;
int left = 0, right = 0, cnt = 0, oldcnt;
static uint8_t peopleCount = 0; //default state: nobody is inside the room
static int resetCounter = 0;
boolean lastTrippedState = 0;

//static int num_timeouts = 0;
double people, distance_avg;

static int PathTrack[] = {0, 0, 0, 0};
static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;
static int zone = 0;

int CurrentZoneStatus = NOBODY;
int AllZonesCurrentStatus = 0;
int AnEventHasOccured = 0;

void sendCounter(int inout, Sensor people_sensor, Sensor distance_sensor)
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
    //client.publish(counter_topic, String(peopleCount).c_str(), true);
    ESP_LOGI("VL53L1X custom sensor", "Sending people count: %d", peopleCount);
    people_sensor.publish_state(peopleCount);
    //distance_sensor->publish_state(distance_avg);

    //Serial.print("PeopleCounter: ");
    //Serial.println(peopleCount);
}

inline void dispUpdate(Sensor people_sensor, Sensor distance_sensor)
{ // 33mS
    // left = rein
    if (left)
    {
        //Serial.println("--->");
        ESP_LOGD("VL53L1X custom sensor", "--->");
        sendCounter(1, people_sensor, distance_sensor);
    }
    // right = raus
    if (right)
    {
        //Serial.println("<---");
        ESP_LOGD("VL53L1X custom sensor", "<---");
        sendCounter(0, people_sensor, distance_sensor);
    }
    //Serial.println(cnt);
    ESP_LOGD("VL53L1X custom sensor", "Count: %d", cnt);
}
int counting(VL53L1XSensor count_sensor, Sensor people_sensor, Sensor distance_sensor)
{

    if (zone == LEFT)
    {
        VL53L1_status = count_sensor.setUserROI(&roiConfig1);
    }
    else
    {
        VL53L1_status = count_sensor.setUserROI(&roiConfig2);
    }
    VL53L1_status = count_sensor.waitMeasurementDataReady();
    if (!VL53L1_status)
        VL53L1_status = count_sensor.getRangingMeasurementData(&RangingData);                        //4mS
    count_sensor.clearInterruptAndEnableNextRange(VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT); //2mS
    if (VL53L1_status == 0)
    {
        distance = RangingData.RangeMilliMeter;
    }

    // 1780
    // 300 mm -> kammer
    //if (distance < DIST_THRESHOLD_MAX_G) {
    if (distance < id(DIST_THRESHOLD_MAX_G))
    {
        // Someone is in !
        CurrentZoneStatus = SOMEONE;
        //ESP_LOGE(TAG, "Global value is: %d", id(DIST_THRESHOLD_MAX_G));
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
                // event in left zone has occured
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
            // need to left right zone as well ...
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
                    if (cnt > 0)
                        cnt--;
                    right = 1;
                    dispUpdate(people_sensor, distance_sensor);
                    right = 0;
                }
                else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
                {
                    // This an entry
                    //PeopleCount ++;
                    cnt++;
                    left = 1;
                    dispUpdate(people_sensor, distance_sensor);
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

    // output debug data to main host machine
    //return(PeopleCount);

    if (resetCounter == 1)
    {
        resetCounter = 0;
        sendCounter(-1, people_sensor, distance_sensor);
    }
}

#endif // #define COUNTER_H