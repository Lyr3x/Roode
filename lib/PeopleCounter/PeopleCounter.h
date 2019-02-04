#ifndef PEOPLECOUNTER_H
#define PEOPLECOUNTER_H
#include <Configuration.h>
#include <Transmitter.h>
#include <SendCounter.h>
int room_sensor_value;     //analog value store for the room sensor
int corridor_sensor_value; //analog value store for the corridor sensor

template <typename T, typename G>
void peoplecounting(T ROOM_SENSOR, T CORRIDOR_SENSOR, G transmitter)
{   
    yield();
    int starttime = millis();
    int endtime = starttime;
    int inout = -1;                        //if inout== 0 -> out; if inout == 1 --> in; THIS STATE WILL BE SENT!
    while ((endtime - starttime) <= LTIME) // do this loop for up to 5000mS
    {
        yield();
        inout = -1;

#ifdef MY_DEBUG
        Serial.print("ROOM_SENSOR:");
        Serial.println(room_sensor_value);
        Serial.print("ROOM_SENSOR THRESHOLD: ");
        Serial.println(ROOM_SENSOR.threshold);
        Serial.print("CORRIDOR_SENSOR:");
        Serial.println(corridor_sensor_value);
        Serial.print("CORRIDOR_SENSOR THRESHOLD: ");
        Serial.println(CORRIDOR_SENSOR.threshold);

#endif

        room_sensor_value = ROOM_SENSOR.readRangeContinuousMillimeters();
        corridor_sensor_value = CORRIDOR_SENSOR.readRangeContinuousMillimeters();

        if (room_sensor_value < ROOM_SENSOR.threshold && corridor_sensor_value > CORRIDOR_SENSOR.threshold && inout != 1)
        {
            int startR = millis();
            int endR = startR;
            while ((endR - startR) <= MTIME)
            {
                yield();
                room_sensor_value = ROOM_SENSOR.readRangeContinuousMillimeters();
                corridor_sensor_value = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                if (corridor_sensor_value < CORRIDOR_SENSOR.threshold && room_sensor_value < ROOM_SENSOR.threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop ROOM_SENSOR: ");
                    Serial.println(room_sensor_value);
                    Serial.print("In Loop CORRIDOR_SENSOR: ");
                    Serial.println(corridor_sensor_value);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endR - startR));
#endif
                    while (room_sensor_value < ROOM_SENSOR.threshold || corridor_sensor_value < CORRIDOR_SENSOR.threshold)
                    {
                        yield();
                        room_sensor_value = ROOM_SENSOR.readRangeContinuousMillimeters();
                        corridor_sensor_value = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                        if (corridor_sensor_value < CORRIDOR_SENSOR.threshold && room_sensor_value > ROOM_SENSOR.threshold)
                        {
                            inout = 0;
                            sendCounter(inout, transmitter);
                            break;
                        }
                    }

                    if (inout == 0)
                    {
                        corridor_sensor_value = 0;
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

        if (corridor_sensor_value < CORRIDOR_SENSOR.threshold && room_sensor_value > ROOM_SENSOR.threshold && inout != 0)
        {
            int startC = millis();
            int endC = startC;
            while ((endC - startC) <= MTIME)
            {
                yield();
                room_sensor_value = ROOM_SENSOR.readRangeContinuousMillimeters();
                corridor_sensor_value = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                if (room_sensor_value < ROOM_SENSOR.threshold && corridor_sensor_value < CORRIDOR_SENSOR.threshold)
                {
#ifdef MY_DEBUG
                    Serial.print("In Loop CORRIDOR_SENSOR: ");
                    Serial.println(corridor_sensor_value);
                    Serial.print("In Loop ROOM_SENSOR: ");
                    Serial.println(room_sensor_value);
                    Serial.print("Delay Time: ");
                    Serial.println(MTIME - (endC - startC));
#endif
                    while (room_sensor_value < ROOM_SENSOR.threshold || corridor_sensor_value < CORRIDOR_SENSOR.threshold)
                    {
                        yield();
                        room_sensor_value = ROOM_SENSOR.readRangeContinuousMillimeters();
                        corridor_sensor_value = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
                        if (room_sensor_value < ROOM_SENSOR.threshold && corridor_sensor_value > CORRIDOR_SENSOR.threshold)
                        {
                            inout = 1;
                            sendCounter(inout, transmitter);
                            break;
                        }
                    }

                    if (inout == 1)
                    {
                        room_sensor_value = 0;
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
    }
}
#endif