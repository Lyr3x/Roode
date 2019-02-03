#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <SendCounter.h>
#include "core/MySensorsCore.h"
#include <Math.h>
#include <Configuration.h>
#include <SensorReader.h>

// int calibration(VL53L0X ROOM_SENSOR, VL53L0X CORRIDOR_SENSOR);
int calculateStandardDeviation(int irValues[]);

template <typename T>
int calibration(T ROOM_SENSOR, T CORRIDOR_SENSOR)
{
    // #ifdef USE_VL53L0X
    // // init() performs all calibration steps again();
    // ROOM_SENSOR.init();
    // CORRIDOR_SENSOR.init();
    // #endif

    int irValues[30] = {};
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(0, 5);
    oled.setTextSize(1, 1);
    oled.println("### Calibrate IR ###");
#endif

#if defined(USE_SHARP_IR)
    digitalWrite(CORRIDOR_ENABLE, HIGH);
    digitalWrite(ROOM_ENABLE, HIGH);
    delay(100);
    auto max = 0;
#elif defined USE_VL53L0X || defined USE_VL53L1X
    auto min = 0;
#endif
    auto n = 0;
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
#if defined(USE_SHARP_IR)
        wait(10);
        irrVal = analogRead(ROOM_SENSOR);
        wait(10);
        ircVal = analogRead(CORRIDOR_SENSOR);

        //calculate the max without jumps for the room sensor
        if (((irrVal > max) && ((irrVal - max) < THRESHOLD_X)) || ((irrVal - max) == irrVal))
        {
            Serial.println(irrVal);
            max = irrVal;
            if (n < 30)
            {
                irValues[n] = max;
                n++;
            }
        }
        //calculate the max without jumps for the corridor sensor
        if (((ircVal > max) && ((ircVal - max) < THRESHOLD_X)) || ((ircVal - max) == ircVal))
        {
            Serial.println(ircVal);
            max = ircVal;
            if (n < 30)
            {
                irValues[n] = max;
                n++;
            }
        }
#elif defined(USE_VL53L0X) || defined USE_VL53L1X
        wait(10);
        irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
        wait(10);
        ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
#ifdef MY_DEBUG
        Serial.println(irrVal);
        Serial.println(ircVal);
#endif
        //calculate the max without jumps for the room sensor
        if ((irrVal < min) || ((irrVal - min) == irrVal))
        {
            Serial.println(irrVal);
            min = irrVal;
            if (n < 30)
            {
                irValues[n] = min;
                n++;
            }
        }
        //calculate the max without jumps for the corridor sensor
        if ((ircVal < min) || ((ircVal - min) == ircVal))
        {
            Serial.println(ircVal);
            min = ircVal;
            if (n < 30)
            {
                irValues[n] = min;
                n++;
            }
        }
#endif
    }
    auto sd = 0;
#if defined USE_SHARP_IR
    // shutdown both sensors
    digitalWrite(CORRIDOR_ENABLE, LOW);
    digitalWrite(ROOM_ENABLE, LOW);
    sd = calculateStandardDeviation(irValues);
    threshold = max + sd;
#elif defined USE_VL53L0X || defined USE_VL53L1X
    sd = calculateStandardDeviation(irValues);
    threshold = min - sd;
#endif

    // Serial.print("standard deviation: " + threshold);
    // threshold = max + THRESHOLD_X;
#ifdef USE_OLED
    oled.setCursor(15, 0);
    oled.print("Threshold: ");
    oled.println(threshold);
    wait(2000);
    oled.clear();
    oled.setCursor(10, 0);
    oled.setTextSize(1, 1);
    oled.println("Calibration done!");
    wait(2000);
#endif
    Serial.print("standard deviation: ");
    Serial.println(sd);
    Serial.print("New threshold is: ");
    Serial.println("#### calibration done ####");

    reportToController(threshold);
    Serial.println(threshold);
    send(thrMsg.set(threshold));

    return threshold;
}

#endif