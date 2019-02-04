#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "Arduino.h"

class Transmitter
{
  public:
    ~Transmitter() {}
    int send();
    int receive();
};
#endif