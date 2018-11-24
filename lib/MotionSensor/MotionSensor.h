#ifndef MotionSensor_h
#define MotionSensor_h

	#include "core/MySensorsCore.h"
	#include "SPI.h" 
	#include "Arduino.h"

	class MotionSensor
	{
		public:
			MotionSensor(int pin);
			~MotionSensor();
			void Setup(int initTime);
			void Presentation();
			int checkMotion();
			int getPin();
		private:
			int _pin;
			int pirState;
			int val;
			int motionInit;
	};
#endif