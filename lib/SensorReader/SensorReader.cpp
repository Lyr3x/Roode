#include "SensorReader.h"
int irrVal = 0;
int ircVal = 0;
int threshold = 180;
void readSensorData()
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
        irrVal = analogRead(ANALOG_IR_SENSORR);
        wait(10);
        ircVal = analogRead(ANALOG_IR_SENSORC);
#elif defined USE_VL53L0X
        ROOM_SENSOR.startContinuous();
        CORRIDOR_SENSOR.startContinuous();
        irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
        ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
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
                irrVal = analogRead(ANALOG_IR_SENSORR);
                wait(10);
                ircVal = analogRead(ANALOG_IR_SENSORC);
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
                        irrVal = analogRead(ANALOG_IR_SENSORR);
                        wait(10);
                        ircVal = analogRead(ANALOG_IR_SENSORC);
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
                ircVal = analogRead(ANALOG_IR_SENSORR);
                wait(10);
                irrVal = analogRead(ANALOG_IR_SENSORC);
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
                        irrVal = analogRead(ANALOG_IR_SENSORR);
                        wait(10);
                        ircVal = analogRead(ANALOG_IR_SENSORC);
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
#elif defined USE_VL53L0X
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