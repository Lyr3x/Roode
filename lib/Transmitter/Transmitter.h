#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#pragma once

#include "Arduino.h"
#include <../Configuration/Config.h>

class Transmitter
{
public:
  ~Transmitter() {}
  int transmit();
  int receive();
  struct Devices
  {
  };
  Devices devices;
};
#endif