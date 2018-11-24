#define MY_DEBUG //Comment out in production mode
#define CALIBRATION  //enables calibration of the irsensors and motion sensor initializing
#define MY_RADIO_NRF24 //use the NRF24L01+ module
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
#include <MySensors.h>
#include <PeopleCounter.h>
PeopleCounter peopleCounter(8,5,7,0,0);
void setup() {
  // put your setup code here, to run once:

}

void presentation(){
  
}

void loop() {
  // put your main code here, to run repeatedly:
  wait(10);
  peopleCounter.setTo(HIGH);
  peopleCounter.setTo(LOW);
  peopleCounter.checkSensors();
}
