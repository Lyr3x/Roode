#ifndef COUNTER_H
#define COUNTER_H
#pragma once

#include <Config.h>
#include <Transmitter.h>
#include <SendCounter.h>

Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData;
int status, i, distance[2] = {0, 0};
int left = 0, right = 0;
long timeMark = 0, DisplayUpdateTime = 0;

inline void dispUpdate()
{ // 33mS
    if (left)
        Serial.println("--->");
    if (right)
        Serial.println("<---");
}
int counting(VL53L1XSensor count_sensor)
{
    
    int gesture_code;

    distance[0] = count_sensor.readRangeContinuoisMillimeters(leftRoiConfig);
    distance[1] = count_sensor.readRangeContinuoisMillimeters(rightRoiConfig);

    gesture_code = tof_gestures_detectDIRSWIPE_1(distance[0], distance[1], &gestureDirSwipeData);
    Serial.printf("%d,%d\n\r", distance[0], distance[1]);
    Serial.println(gesture_code);
    switch (gesture_code)
    {
    case GESTURES_SWIPE_LEFT_RIGHT:
        right = 1;
        dispUpdate();
        right = 0;
        return sendCounter(1, transmitter);
    //   break;
    case GESTURES_SWIPE_RIGHT_LEFT:
        left = 1;
        dispUpdate();
        left = 0;
        return sendCounter(0, transmitter);
    //   break;
    default:
        break;
    }
}

#endif // #define COUNTER_H