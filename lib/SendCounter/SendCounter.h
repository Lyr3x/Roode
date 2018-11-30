#ifndef SENDCOUNTER_H
#define SENDCOUNTER_H
#include "core/MySensorsCore.h"
#include <Configuration.h>
#include <Communication.h>

void sendCounter(int inout);
extern int peopleCount; //default state: nobody is inside the room
#endif