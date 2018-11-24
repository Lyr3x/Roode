#include "PeopleCounter.h"

#define MTIME 500
#define LTIME 10000
#define CALIBRATION_VAL 500
int _threshold = 0;
int irrVal = 0;
int ircVal = 0;
int inout = -1;
int ROOM_ANALOG = -1;
int CORRIDOR_ANALOG = -1;
int ROOM_ENABLE = -1;
int CORRIDOR_ENABLE = -1;
PeopleCounter::PeopleCounter(int corridor_enable, int corridor_analog, int room_enable, int room_analog)
{
	// //set variables to function parameters
	ROOM_ANALOG = room_analog;
	CORRIDOR_ANALOG = corridor_analog;
	ROOM_ENABLE = room_enable;
	CORRIDOR_ENABLE = corridor_enable;

	// //set enable pins of the ir sensors to OUTPUT pinmode
	pinMode(ROOM_ENABLE, OUTPUT);
	pinMode(CORRIDOR_ENABLE, OUTPUT);

	// //check if threshold needs to be set to the default (160)
}

PeopleCounter::~PeopleCounter(){}

void PeopleCounter::Setup(int threshold){
  if(threshold == 0){
    _threshold = calibration();
  }else{
    _threshold = threshold;
  }
}

void PeopleCounter::Presentation(){
}

void PeopleCounter::setTo(int high_low){
  digitalWrite(ROOM_ENABLE, high_low);
  wait(1);
  digitalWrite(CORRIDOR_ENABLE, high_low);
}

int PeopleCounter::checkSensors(){
	int starttime = millis();
  int endtime = starttime;

  while ((endtime - starttime) <= LTIME) // do this loop for up to 5000mS
  {
    // turn both sensors on
    digitalWrite(ROOM_ENABLE, HIGH);
    wait(1);
    digitalWrite(CORRIDOR_ENABLE, HIGH);
    wait(5);
    inout = -1;

    irrVal = analogRead(ROOM_ANALOG);
    wait(10);
    ircVal = analogRead(CORRIDOR_ANALOG);

#ifdef MY_DEBUG
    Serial.print("IRR:" );
    Serial.println(irrVal);
    Serial.print("IRC:" );
    Serial.println(ircVal);
#endif
    if (irrVal > _threshold && ircVal < _threshold && inout != 1) {
      int startR = millis();
      int endR = startR;
      while ((endR - startR) <= MTIME) {
        irrVal = analogRead(ROOM_ANALOG);
        wait(10);
        ircVal = analogRead(CORRIDOR_ANALOG);
        if (ircVal > _threshold && irrVal > _threshold) {
#ifdef MY_DEBUG
          Serial.print("In Loop IRR: ");
          Serial.println(irrVal);
          Serial.print("In Loop IRC: ");
          Serial.println(ircVal);
          Serial.print("Delay Time: ");
          Serial.println(MTIME - (endR - startR));
#endif
          while (irrVal > _threshold || ircVal > _threshold) {
            irrVal = analogRead(ROOM_ANALOG);
            wait(10);
            ircVal = analogRead(CORRIDOR_ANALOG);
            if (ircVal > _threshold && irrVal < _threshold) {
              // turn both sensors off
              digitalWrite(ROOM_ENABLE, LOW);
              wait(1);
              digitalWrite(CORRIDOR_ENABLE, LOW);
              inout = 0;
              return inout;
              // break;
            }
            wait(11);
          }

          if (inout == 0) {
            wait(150);
            ircVal = 0;
            endR = millis();
            starttime = millis();
            endtime = starttime;
            break;
          }
        } else {
          wait(1);
          endR = millis();
        }
      }
    } else {
      endtime = millis();
    }

    if (ircVal > _threshold && irrVal < _threshold && inout != 0) {
      int startC = millis();
      int endC = startC;
      while ((endC - startC) <= MTIME) {
        ircVal = analogRead(CORRIDOR_ANALOG);
        wait(10);
        irrVal = analogRead(ROOM_ANALOG);
        if (irrVal > _threshold && ircVal > _threshold) {
#ifdef MY_DEBUG
          Serial.print("In Loop IRC: ");
          Serial.println(ircVal);
          Serial.print("In Loop IRR: ");
          Serial.println(irrVal);
          Serial.print("Delay Time: ");
          Serial.println(MTIME - (endC - startC));
#endif
          while (irrVal > _threshold || ircVal > _threshold) {
            irrVal = analogRead(ROOM_ANALOG);
            wait(10);
            ircVal = analogRead(CORRIDOR_ANALOG);
            if (irrVal > _threshold && ircVal < _threshold) {
              // turn both sensors off
              digitalWrite(ROOM_ENABLE, LOW);
              wait(1);
              digitalWrite(CORRIDOR_ENABLE, LOW);
              inout = 1;
              return inout;
              // break;
            }
            wait(11);
          }

          if (inout == 1) {
            wait(150);
            irrVal = 0;
            endC = millis();
            starttime = millis();
            endtime = starttime;
            break;
          }
        } else {
          wait(11);
          endC = millis();
        }
      }
    } else {
      endtime = millis();
    }
    wait(11);
  }
  return -1;
}


int PeopleCounter::calibration() {

  Serial.println("##### Begin IR-Sensor calibration #####");

  digitalWrite(CORRIDOR_ENABLE, HIGH);
  digitalWrite(ROOM_ENABLE, HIGH);
  delay(100);
  int distances[CALIBRATION_VAL];
  int max = 0;
  for (int m = 0; m < CALIBRATION_VAL; m = m + 2) {
    delay(8);
    irrVal = analogRead(ROOM_ANALOG);
    delay(8);
    ircVal = analogRead(CORRIDOR_ANALOG);

    //calculate the max withut jumps for the room sensor
    if (irrVal >= max && (irrVal - max) < 30 || irrVal - max == irrVal) {
      Serial.println(irrVal);
      max = irrVal;
      distances[m] = irrVal;
    } else {
      distances[m] = 0;
    }

    //calculate the max withut jumps for the corridor sensor
    if (ircVal >= max && (ircVal - max) < 30 || ircVal - max == ircVal) {
      Serial.println(ircVal);
      max = ircVal;
      distances[m + 1] = ircVal;
    } else {
      distances[m + 1] = 0;
    }
  }

  // shutdown both sensors
  digitalWrite(CORRIDOR_ENABLE, LOW);
  digitalWrite(ROOM_ENABLE, LOW);
  max = 0;
  //calculate the absolute max value to determine the new threshold
  for (int i = 0; i < CALIBRATION_VAL; i++) {
    if (distances[i] >= max) {
      max = distances[i];
    }
  }
  Serial.println("##### IR-Sensor calibration done #####");
  Serial.print("##### New threshold: #####");
  Serial.println(max+30);
  return max+30; //add 30 to be super safe
}
