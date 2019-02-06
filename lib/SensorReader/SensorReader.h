#ifndef SENSORREADER_H
#define SENSORREADER_H
// #include <Configuration.h>
#include <SendCounter.h>
#include "core/MySensorsCore.h"
// template <typename T>
// void readSensorData(T ROOM_SENSOR, T CORRIDOR_SENSOR);
// void readSensorData(VL53L0X ROOM_SENSOR, VL53L0X CORRIDOR_SENSOR);
// void readSensorData();
// some needed var declarations
extern int irrVal;    //analog value store for the room sensor
extern int ircVal;    //analog value store for the corridor sensor
extern int threshold; //if CALIBRATION is not defined, this threshold is used (okay for a 80cm doorway using reflection foil or white paper)
template <typename T>
void readSensorData(T ROOM_SENSOR, T CORRIDOR_SENSOR)
{
    int starttime = millis();
    int endtime = starttime;
    int inout = -1;                        //if inout== 0 -> out; if inout == 1 --> in; THIS STATE WILL BE SENT!
    while ((endtime - starttime) <= LTIME) // do this loop for up to 5000mS
    {
        inout = -1;
#if defined USE_SHARP_IR
        // turn both sensors on
        digitalWrite(ROOM_ENABLE, HIGH);
        wait(1);
        digitalWrite(CORRIDOR_ENABLE, HIGH);
        wait(5);
        irrVal = analogRead(ROOM_SENSOR);
        wait(10);
        ircVal = analogRead(CORRIDOR_SENSOR);

#endif

#ifdef MY_DEBUG
        Serial.print("IRR:");
        Serial.println(irrVal);
        Serial.print("IRC:");
        Serial.println(ircVal);
#endif

#if defined USE_SHARP_IR
        if (irrVal > threshold && ircVal < threshold && inout != 1)
        {
            int startR = millis();
            int endR = startR;
            while ((endR - startR) <= MTIME)
            {
                irrVal = analogRead(ROOM_SENSOR);
                wait(10);
                ircVal = analogRead(CORRIDOR_SENSOR);
                if (ircVal > threshold && irrVal > threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop IRR: ");
                    Serial.println(irrVal);
                    Serial.print("In Loop IRC: ");
                    Serial.println(ircVal);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endR - startR));
#endif
                    while (irrVal > threshold || ircVal > threshold)
                    {
                        irrVal = analogRead(ROOM_SENSOR);
                        wait(10);
                        ircVal = analogRead(CORRIDOR_SENSOR);
                        if (ircVal > threshold && irrVal < threshold)
                        {
                            // turn both sensors off
                            digitalWrite(ROOM_ENABLE, LOW);
                            wait(1);
                            digitalWrite(CORRIDOR_ENABLE, LOW);
                            inout = 0;
                            sendCounter(inout);
                            break;
                        }
                        wait(11);
                    }

                    if (inout == 0)
                    {
                        wait(150);
                        ircVal = 0;
                        endR = millis();
                        starttime = millis();
                        endtime = starttime;
                        break;
                    }
                }
                else
                {
                    wait(1);
                    endR = millis();
                }
            }
        }
        else
        {
            endtime = millis();
        }
        if (ircVal > threshold && irrVal < threshold && inout != 0)
        {
            int startC = millis();
            int endC = startC;
            while ((endC - startC) <= MTIME)
            {
                ircVal = analogRead(ROOM_SENSOR);
                wait(10);
                irrVal = analogRead(CORRIDOR_SENSOR);
                if (irrVal > threshold && ircVal > threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop IRC: ");
                    Serial.println(ircVal);
                    Serial.print("In Loop IRR: ");
                    Serial.println(irrVal);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endC - startC));
#endif
                    while (irrVal > threshold || ircVal > threshold)
                    {
                        irrVal = analogRead(ROOM_SENSOR);
                        wait(10);
                        ircVal = analogRead(CORRIDOR_SENSOR);
                        if (irrVal > threshold && ircVal < threshold)
                        {
                            // turn both sensors off
                            digitalWrite(ROOM_ENABLE, LOW);
                            wait(1);
                            digitalWrite(CORRIDOR_ENABLE, LOW);

                            inout = 1;
                            sendCounter(inout);
                            break;
                        }
                        wait(11);
                    }

                    if (inout == 1)
                    {
                        wait(150);
                        irrVal = 0;
                        endC = millis();
                        starttime = millis();
                        endtime = starttime;
                        break;
                    }
                }
                else
                {
                    wait(11);
                    endC = millis();
                }
            }
        }
        else
        {
            endtime = millis();
        }
        wait(10);
#elif defined USE_VL53L0X || defined USE_VL53L1X
        irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
        ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
        if (irrVal < threshold && ircVal > threshold && inout != 1)
        {
            int startR = millis();
            int endR = startR;
            while ((endR - startR) <= MTIME)
            {
                irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
                ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                if (ircVal < threshold && irrVal < threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop IRR: ");
                    Serial.println(irrVal);
                    Serial.print("In Loop IRC: ");
                    Serial.println(ircVal);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endR - startR));
#endif
                    while (irrVal < threshold || ircVal < threshold)
                    {
                        irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
                        ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                        if (ircVal < threshold && irrVal > threshold)
                        {
                            inout = 0;
                            sendCounter(inout);
                            break;
                        }
                    }

                    if (inout == 0)
                    {
                        //wait(150);
                        ircVal = 0;
                        endR = millis();
                        starttime = millis();
                        endtime = starttime;
                        break;
                    }
                }
                else
                {
                    endR = millis();
                }
            }
        }
        else
        {
            endtime = millis();
        }

        if (ircVal < threshold && irrVal > threshold && inout != 0)
        {
            int startC = millis();
            int endC = startC;
            while ((endC - startC) <= MTIME)
            {
                irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
                ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                if (irrVal < threshold && ircVal < threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop IRC: ");
                    Serial.println(ircVal);
                    Serial.print("In Loop IRR: ");
                    Serial.println(irrVal);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endC - startC));
#endif
                    while (irrVal < threshold || ircVal < threshold)
                    {
                        irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
                        ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                        if (irrVal < threshold && ircVal > threshold)
                        {
                            inout = 1;
                            sendCounter(inout);
                            break;
                        }
                        //wait(11);
                    }

                    if (inout == 1)
                    {
                        //wait(150);
                        irrVal = 0;
                        endC = millis();
                        starttime = millis();
                        endtime = starttime;
                        break;
                    }
                }
                else
                {
                    endC = millis();
                }
            }
        }
        else
        {
            endtime = millis();
        }
#endif
    }
}
#endif