#ifndef CALIBRATION_HPP
#define CALIBRATION_HPP
#include <Configuration.h>
#include <Sensor.h>
#include <Math.h>

int calculateStandardDeviation(int irValues[]);

// template <typename T>
// int calibration(T SENSOR )
// {
//     int irValues[30] = {};
// #ifdef USE_OLED
//     oled.clear();
//     oled.setCursor(0, 5);
//     oled.setTextSize(1, 1);
//     oled.println("### Calibrate IR ###");
// #endif
//     auto min = 0;

//     auto n = 0;
//     for (int m = 0; m < CALIBRATION_VAL; m++)
//     {
//         wait(10);
//         auto sensor_value = SENSOR.readRangeContinuousMillimeters();

// #ifdef MY_DEBUG
//         Serial.println(sensor_value);
// #endif
//         //calculate the max without jumps for the room sensor
//         if ((sensor_value < min) || ((sensor_value - min) == sensor_value))
//         {
//             Serial.println(sensor_value);
//             min = sensor_value;
//             if (n < 30)
//             {
//                 irValues[n] = min;
//                 n++;
//             }
//         }
//     }
//     auto sd = 0;

//     sd = calculateStandardDeviation(irValues);
//     auto threshold = min - sd;

// // Serial.print("standard deviation: " + threshold);
// // threshold = max + THRESHOLD_X;
// #ifdef USE_OLED
//     oled.setCursor(15, 0);
//     oled.print("Threshold: ");
//     oled.println(threshold);
//     wait(2000);
//     oled.clear();
//     oled.setCursor(10, 0);
//     oled.setTextSize(1, 1);
//     oled.println("Calibration done!");
//     wait(2000);
// #endif
//     Serial.print("standard deviation: ");
//     Serial.println(sd);
//     Serial.print("New threshold is: ");
//     Serial.println("#### calibration done ####");
//     Serial.println(threshold);
//     //send(thrMsg.set(threshold)); //REWORK

//     return threshold;
// }

#endif