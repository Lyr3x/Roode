#ifndef PeopleCounter_h
#define PeopleCounter_h

#include "core/MySensorsCore.h"
#include "SPI.h" 
#include "Arduino.h"
class PeopleCounter
{
  public:
    PeopleCounter(int corridor_enable, int corridor_analog, int room_enable, int room_analog);
    ~PeopleCounter();
    void Setup(int threshold);
    void Presentation();
    int checkSensors();
    void setTo(int high_low);
  private:
    int calibration();
    int ROOM_ANALOG;
    int CORRIDOR_ANALOG;
    int ROOM_ENABLE;
    int CORRIDOR_ENABLE;
    int _threshold;
    int inout;
    int irrVal;
    int ircVal;
};

#endif