#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Configuration.h>
#include "core/MySensorsCore.h"
/* MySensors Message types and default settings */
extern unsigned long SLEEP_TIME; //sleep forever
#define CHILD_ID_R 0
#define CHILD_ID_PC 1
#define CHILD_ID_THR 3
static MyMessage msg(CHILD_ID_R, V_STATUS);    //room on/off child
static MyMessage pcMsg(CHILD_ID_PC, V_TEXT);   //people counter child
static MyMessage thrMsg(CHILD_ID_THR, V_TEXT); //Threshold child
#endif