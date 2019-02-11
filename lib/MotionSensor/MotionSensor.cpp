#include "MotionSensor.h"

MotionSensor::MotionSensor(int pin)
{
	_pin = pin;
	pinMode(_pin, INPUT);
	pirState = LOW;
	val = 0;
}

MotionSensor::~MotionSensor(){}

void MotionSensor::Setup(int initTime){
	Serial.println(F("##### Begin Motion Sensor initializing #####"));
	if(initTime > 0){
		for(int i = 0; i <= initTime; i++){
			delay(1000);
		}
	}
	Serial.println(F("##### Motion Sensor initializing done! #####"));

}

void MotionSensor::Presentation(){
}

int MotionSensor::checkMotion()
{
	//Motion Sensor
  val = digitalRead(_pin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {      // check if the pir status is LOW
      pirState = HIGH;          // set pir status to HIGH
    }
  } else {
    if (pirState == HIGH) {     //check if the pir status is HIGH
      pirState = LOW;           // set the pir status to LOW
    }
  }
  return pirState;
}

int MotionSensor::getPin(){
	return _pin;
}