/*
People Counting Using a Single ST Time-of-Flight Sensor
Copyright 2019 Vladimir I. Yershov vlad.yer@gmail.com

This example illustrates direction recognition with the
single VL53L1X TOF sensor. Two Regions Of Interest(ROI) 
are defined in SPAD sensor array:
  ROI1	  ROI2
0,15	12,15 
  ####----####
  ####----####
  ####----####
  ...
  ####----####
  ####----####
	3,0		15,0
Data measurements distance0 and distance1 cyclically readed
from both ROIs.
Then STM32duino_Proximity_Gesture library used for event 
interpretation.
NOTE:
tof_gestures_DIRSWIPE_1.cpp and tof_gestures_DIRSWIPE_1.h
need to be modified for  handMustCoverBoth parameter support

This code is for WEMOS D1 mini Lite, but with board and pins
adjustments will work with any Arduino
*/

#include <Wire.h>
#include "vl53l1_api.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <tof_gestures.h>
#include <tof_gestures_DIRSWIPE_1.h>

#define SDA			2	// D4
#define SCL			5	// D1
#define XSHUT		14	// D5
#define INT			D6

#define light_on	0	//D3
#define light_off	4	//D2

#define dev1_sel	digitalWrite(XSHUT, HIGH);
#define dev1_desel	digitalWrite(XSHUT, LOW);

VL53L1_Dev_t		sensor;
VL53L1_DEV			Dev = &sensor;

// Two ROI configurations
VL53L1_UserRoi_t	roiConfig1 = { 12, 15, 15, 0 };
VL53L1_UserRoi_t	roiConfig2 = { 0, 15, 3, 0 };

Adafruit_SSD1306 display;

Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData;
int status, i, distance[2] = { 0, 0 };
int left = 0, right = 0, cnt = 0, oldcnt;
volatile int LightON = 0, OLED_dimmed = 0, OLED_OFF_timeout = 10000;
long timeMark = 0, DisplayUpdateTime = 0;

void setup()
{
	Wire.begin(SDA, SCL);
	Wire.setClock(400000);
	Serial.begin(115200);

	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

	display.clearDisplay();
	display.setTextSize(4);
	display.setTextColor(WHITE, BLACK);
	display.setCursor(0, 0);
	display.print("START");
	display.display();

	pinMode(XSHUT, OUTPUT);

	delay(100);

	dev1_sel

	Dev->I2cDevAddr = 0x52;

	Serial.printf("\n\rDevice data  ");
	checkDev(Dev);
	delay(1000);

	// with modified DIRDWIPE lib
	tof_gestures_initDIRSWIPE_1(800, 0, 1000, false, &gestureDirSwipeData);
	// with NOT modified DIRDWIPE lib
	//	tof_gestures_initDIRSWIPE_1(800, 0, 1000, &gestureDirSwipeData);

	status += VL53L1_WaitDeviceBooted(Dev);
	status += VL53L1_DataInit(Dev);
	status += VL53L1_StaticInit(Dev);
	status += VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_SHORT);
	status += VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 10000);	// 73Hz
	status += VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 15); 
	if (status) {
		Serial.printf("StartMeasurement failed status: %d\n\r", status);
	  }
  
	VL53L1_StartMeasurement(Dev);
}

void loop() {
	static VL53L1_RangingMeasurementData_t RangingData;
	int gesture_code;

	if (((millis() - DisplayUpdateTime) > OLED_OFF_timeout) && !OLED_dimmed) {  // неправильно все время выполняется
//		Serial.println("Display OFF");
		display.clearDisplay();
		display.display();
		OLED_dimmed = 1;
	}

	status = VL53L1_SetUserROI(Dev, &roiConfig1);

	while (digitalRead(INT));	// slightly faster
//	status = VL53L1_WaitMeasurementDataReady(Dev);
	if (!status) status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);	//4mS
	VL53L1_clear_interrupt_and_enable_next_range(Dev, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);	//2mS
	if (status == 0) distance[0] = RangingData.RangeMilliMeter;

	status = VL53L1_SetUserROI(Dev, &roiConfig2);

	while (digitalRead(INT));	// slightly faster
//	status = VL53L1_WaitMeasurementDataReady(Dev);
	if (!status) status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);	//4mS
	VL53L1_clear_interrupt_and_enable_next_range(Dev, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);	//2mS
	if (status == 0) distance[1] = RangingData.RangeMilliMeter;

	gesture_code = tof_gestures_detectDIRSWIPE_1(distance[0], distance[1], &gestureDirSwipeData);	//0mS

	switch (gesture_code)
	{
	case GESTURES_SWIPE_LEFT_RIGHT:
		cnt++;
		right = 1;
		break;
	case GESTURES_SWIPE_RIGHT_LEFT:
		cnt--;
		left = 1;
		break;
	default:
		break;
	}

	if (distance[0] < 800 || distance[1] < 800) { timeMark = millis(); }

	if (cnt != oldcnt) {
		oldcnt = cnt;
		if (cnt < 0) {
			LightON = 1;
			lighton();
		}
		else {
			LightON = 0;
			lightoff();
		}
		dispUpdate();
		left = 0; right = 0;
	}

	if (millis() - timeMark < 500) {
		// suitable to use with SerialPlot:  https://bitbucket.org/hyOzd/serialplot
		Serial.printf("%d,%d,%d\n\r", distance[0], distance[1], cnt * 500);
	}
}

void checkDev(VL53L1_DEV Dev) {
	uint16_t wordData;
	VL53L1_RdWord(Dev, 0x010F, &wordData);
	Serial.printf("DevAddr: 0x%X VL53L1X: 0x%X\n\r", Dev->I2cDevAddr, wordData);
}

inline void dispUpdate() {	// 33mS
	OLED_dimmed = 0;
	display.clearDisplay();
	display.setCursor(0, 0);
	if (right)display.println("--->");
	else display.println("<---");
	display.print(cnt);
	if (LightON)	display.print(" * ");
	else display.print("   ");
	display.display();
	DisplayUpdateTime = millis();
}

void lighton(void) {
	pinMode(light_on, OUTPUT);
	digitalWrite(light_on, LOW);
	delay(100);
	pinMode(light_on, INPUT);
}

void lightoff(void) {
	pinMode(light_off, OUTPUT);
	digitalWrite(light_off, LOW);
	delay(100);
	pinMode(light_off, INPUT);
}
