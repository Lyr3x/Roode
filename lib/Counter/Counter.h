#ifndef COUNTER_H
#define COUNTER_H
#pragma once

#include <Config.h>
#include <Transmitter.h>
#include <SendCounter.h>
VL53L1_UserRoi_t roiConfig1 = {10, 15, 15, 0}; //TopLeftX, TopLeftY, BotRightX, BotRightY
VL53L1_UserRoi_t roiConfig2 = {0, 15, 5, 0};   //TopLeftX, TopLeftY, BotRightX, BotRightY
Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData;
int status, i, distance[2] = {0, 0};
int left = 0, right = 0, cnt = 0, oldcnt;
long timeMark = 0, DisplayUpdateTime = 0;

inline void dispUpdate() {	// 33mS
  if (left)Serial.println("--->");
  if (right) Serial.println("<---");
  Serial.println(cnt);
}
void counting(VL53L1_DEV count_sensor){
    static VL53L1_RangingMeasurementData_t RangingData;
  int gesture_code;


  status = VL53L1_SetUserROI(count_sensor, &roiConfig1);

  //while (digitalRead(INT));	// slightly faster
  status = VL53L1_WaitMeasurementDataReady(count_sensor);
  if (!status) status = VL53L1_GetRangingMeasurementData(count_sensor, &RangingData);	//4mS
  VL53L1_clear_interrupt_and_enable_next_range(count_sensor, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);	//2mS
  if (status == 0) distance[0] = RangingData.RangeMilliMeter;

  status = VL53L1_SetUserROI(count_sensor, &roiConfig2);

  //while (digitalRead(INT));	// slightly faster
  status = VL53L1_WaitMeasurementDataReady(count_sensor);
  if (!status) status = VL53L1_GetRangingMeasurementData(count_sensor, &RangingData);	//4mS
  VL53L1_clear_interrupt_and_enable_next_range(count_sensor, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);	//2mS
  if (status == 0) distance[1] = RangingData.RangeMilliMeter;

  gesture_code = tof_gestures_detectDIRSWIPE_1(distance[0], distance[1], &gestureDirSwipeData);	//0mS
  //Serial.printf("%d,%d,%d\n\r", distance[0], distance[1], cnt);
  switch (gesture_code)
  {
    case GESTURES_SWIPE_LEFT_RIGHT:
      if (cnt > 0)
        cnt--;
      right = 1;
      dispUpdate();
      right = 0;
      break;
    case GESTURES_SWIPE_RIGHT_LEFT:
      cnt++;
      left = 1;
      dispUpdate();
      left = 0;
      break;
    default:
      break;
  }
}

#endif // #define COUNTER_H