#include "BatteryMeter.h"

int oldBatteryPcnt = 0;
long oldvoltage = 0;
int batteryVoltage = 0;
int batteryPcnt = 1;
float voltage = 0;
float vccReference = 3.3;

BatteryMeter::BatteryMeter(int pin)
{
	voltagePin = pin;
}

// float BatteryMeter::checkBatteryLevel()
// {
//   sensorValue = analogRead(_pin);
// 	batteryPcnt = sensorValue / 10;
// 	voltage = sensorValue * (5.00/1023.00);

// 	if(oldvoltage != voltage){
// 		oldvoltage = voltage;
// 		return voltage;
// 	}else{
// 		return oldvoltage;
// 	}
// }

float BatteryMeter::checkBatteryLevel(){
	float batteryVoltage = ((float)analogRead(voltagePin)*vccReference/1024)*2;
	return batteryVoltage;
}

/*
Voltage to percentage

const float  vccMin = 1.0*3.5;
const float vccMax = 1.0*4.2;
if(batteryVoltage > vccMin){
		batteryPcnt = 100.0*(batteryVoltage - vccMin)/(vccMax - vccMin);
	}

*/

