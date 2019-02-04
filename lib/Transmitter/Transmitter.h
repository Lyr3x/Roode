#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "Arduino.h"
#include <Configuration.h>

class Transmitter
{
public:
  ~Transmitter() {}
  int transmit();
  int receive();
  struct Devices{};
  Devices devices;
};
#endif