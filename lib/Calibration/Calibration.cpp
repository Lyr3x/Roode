#include "Calibration.h"

int calculateStandardDeviation(int irValues[])
{
    auto sumOfValues = 0;
    auto arrLength = 0;
    for (int i = 0; i < 30; i++)
    {
        if (irValues[i] != 0)
        {
            sumOfValues += irValues[i];
            arrLength++;
        }
    }

    auto meanValue = sumOfValues / arrLength;

    auto standardDeviation = 0;
    for (int i = 0; i < arrLength; ++i)
    {
        standardDeviation += pow(irValues[i] - meanValue, 2);
    }
    standardDeviation /= arrLength;

    standardDeviation = sqrt(standardDeviation);

    return standardDeviation;
}

// First step: Sensor calibration
// Second step: threshold calibration
int calibration(VL53L0X ROOM_SENSOR, VL53L0X CORRIDOR_SENSOR)
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
#elif defined USE_VL53L0X
    auto min = 0;
#endif
    auto n = 0;
    for (int m = 0; m < CALIBRATION_VAL; m++)
    {
#if defined(USE_SHARP_IR)
        wait(10);
        irrVal = analogRead(ANALOG_IR_SENSORR);
        wait(10);
        ircVal = analogRead(ANALOG_IR_SENSORC);

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
#elif defined(USE_VL53L0X)
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
#elif defined USE_VL53L0X
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
    Serial.println(threshold);
    send(thrMsg.set(threshold));
    Serial.println("#### calibration done ####");
    return threshold;
}