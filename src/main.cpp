/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/

#include <Configuration.h>
#include <MySensors.h>    // include the MySensors library
#include <Arduino.h>      //need to be included, cause the file is moved to a .cpp file
#include <MotionSensor.h> //MotionSensorLib
#include <BatteryMeter.h> //Include and Set Up BatteryMeter Library
#include <SensorReader.h>
#include <Communication.h>
#include <Calibration.h>

// battery setup
#ifdef USE_BATTERY
BatteryMeter battery(BATTERY_METER_PIN);            //BatteryMeter instance
MyMessage voltage_msg(CHILD_ID_BATTERY, V_VOLTAGE); //MySensors battery voltage message instance
#endif

// /* MySensors Message types and default settings */
// unsigned long SLEEP_TIME = 0; //sleep forever
// #define CHILD_ID_R 0
// #define CHILD_ID_PC 1
// #define CHILD_ID_THR 3
// MyMessage msg(CHILD_ID_R, V_STATUS);    //room on/off child
// MyMessage pcMsg(CHILD_ID_PC, V_TEXT);   //people counter child
// MyMessage thrMsg(CHILD_ID_THR, V_TEXT); //Threshold child

// function prototypes
// void readSensorData();
// int calibration();
// void sendCounter(int inout);
void readCounterButtons();

void setup()
{
#ifdef USE_OLED
  oled.init();
  oled.clear();
  oled.setTextSize(1);
  oled.setCursor(0, 25);
  oled.println("### Roode ###");
  oled.setCursor(10, 0);
  oled.print("RooDe: ");
  oled.print(ROODE_VERSION);
  oled.print("\n");
  oled.print("MySensors: ");
  oled.println(MYSENSORS_LIBRARY_VERSION);
  wait(2000);
#endif

  Serial.println("##### RooDe Presence Detection System #####");

#if defined(USE_SHARP_IR) && !defined(USE_VL53L0X)
  //Corridor Sensor Enable PIN
  pinMode(CORRIDOR_ENABLE, OUTPUT);

  //Room Sensor Voltage Enable PIN
  pinMode(ROOM_ENABLE, OUTPUT);

  //Motion Sensor
  pinMode(DIGITAL_INPUT_SENSOR, INPUT); // declare motionsensor as input
#endif
#if !defined(USE_SHARP_IR) && defined(USE_VL53L0X)
  pinMode(ROOM_ENABLE, OUTPUT);
  pinMode(CORRIDOR_ENABLE, OUTPUT);
  Wire.begin();

  pinMode(ROOM_ENABLE, INPUT);
  delay(10);
  ROOM_SENSOR.setAddress(ROOM_SENSOR_newAddress);
  pinMode(CORRIDOR_ENABLE, INPUT);
  delay(10);
  CORRIDOR_SENSOR.setAddress(CORRIDOR_SENSOR_newAddress);

  ROOM_SENSOR.init();
  CORRIDOR_SENSOR.init();
  ROOM_SENSOR.setTimeout(500);
  CORRIDOR_SENSOR.setTimeout(500);
  ROOM_SENSOR.startContinuous();
  CORRIDOR_SENSOR.startContinuous();

#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  ROOM_SENSOR.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  ROOM_SENSOR.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  ROOM_SENSOR.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  // lower the return signal rate limit (default is 0.25 MCPS)
  CORRIDOR_SENSOR.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  CORRIDOR_SENSOR.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  CORRIDOR_SENSOR.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  ROOM_SENSOR.setMeasurementTimingBudget(20000);
  CORRIDOR_SENSOR.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  ROOM_SENSOR.setMeasurementTimingBudget(200000);
  CORRIDOR_SENSOR.setMeasurementTimingBudget(200000);
#endif
#endif

#if defined(USE_VL53L1X)
  pinMode(ROOM_ENABLE, OUTPUT);
  pinMode(CORRIDOR_ENABLE, OUTPUT);
  Wire.begin();

  pinMode(ROOM_ENABLE, INPUT);
  delay(10);
  ROOM_SENSOR.setAddress(ROOM_SENSOR_newAddress);
  pinMode(CORRIDOR_ENABLE, INPUT);
  delay(10);
  CORRIDOR_SENSOR.setAddress(CORRIDOR_SENSOR_newAddress);

  ROOM_SENSOR.init();
  CORRIDOR_SENSOR.init();
  ROOM_SENSOR.setTimeout(500);
  CORRIDOR_SENSOR.setTimeout(500);
  ROOM_SENSOR.startContinuous(33);
  CORRIDOR_SENSOR.startContinuous(33);

#if defined LONG_RANGE
  // Short, Medium, Long, Unkown as ranges are possible
  ROOM_SENSOR.setDistanceMode(VL53L1X::Long);
  ROOM_SENSOR.setMeasurementTimingBudget(33000);
  CORRIDOR_SENSOR.setDistanceMode(VL53L1X::Long);
  CORRIDOR_SENSOR.setMeasurementTimingBudget(33000);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif
#endif

#ifdef CALIBRATION
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(0, 0);
  oled.setTextSize(1, 1);
  oled.println("# Calibrate Motion #");
#endif
  motion.Setup(MOTION_INIT_TIME);
  // Serial.println("#### motion sensor initialized ####");

  Serial.println("#### calibrate the ir sensors ####");

  calibration();
#endif

  Serial.println("#### Setting the PresenceCounter and Status to OUT (0) ####");
  send(msg.set(0));   //Setting presence status to 0
  send(pcMsg.set(0)); //Setting the people counter to 0
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(10, 0);
  oled.println("Setting Counter to 0");
  wait(1000);
#endif

#ifdef USE_COUNTER_BUTTONS
  pinMode(INCREASE_BUTTON, INPUT); // declare the counter increase button as input
  pinMode(DECREASE_BUTTON, INPUT); // declare the counter decrease button as input
#endif
}

void presentation()
{
  sendSketchInfo("RooDe", ROODE_VERSION);
  present(CHILD_ID_R, S_BINARY);
  present(CHILD_ID_PC, S_INFO);
#ifdef USE_BATTERY
  present(CHILD_ID_BATTERY, S_CUSTOM);
#endif
  present(CHILD_ID_THR, S_INFO);
}

void receive(const MyMessage &message)
{
  if (message.type == V_TEXT)
  {
    Serial.println("V_TEXT update");
    Serial.print("MySensor message received:");
    Serial.println(message.sensor);
    Serial.println(message.type);
    Serial.println(message.sender);
    Serial.println(message.getString());
    String newThreshold = message.getString();
    if (message.sensor == 3 && newThreshold.substring(0, 11) == "recalibrate")
    {
      calibration();
    }

    if (message.sensor == CHILD_ID_PC)
    {
      Serial.println(message.getInt());
      peopleCount = message.getInt();
      send(pcMsg.set(peopleCount));
    }
  }
}
int lastState = LOW;
int newState = LOW;

void loop()
{

  // Sleep until interrupt comes in on motion sensor. Send never an update
  if (motion.checkMotion() == LOW)
  {
#ifdef USE_OLED
    oled.clear();
#endif
#ifdef USE_ENERGY_SAVING
    if (lastState == HIGH)
    {
      readSensorData(); //One more tracking phase before do some powersaving
#if defined USE_SHARP_IR
      digitalWrite(ROOM_ENABLE, LOW);
      wait(1);
      digitalWrite(CORRIDOR_ENABLE, LOW);
#elif defined USE_VL53L0X || defined USE_VL53L1X
      ROOM_SENSOR.stopContinuous();
      CORRIDOR_SENSOR.stopContinuous();
#endif
      request(CHILD_ID_THR, V_TEXT, 0);
      request(CHILD_ID_PC, V_TEXT, 0);
      lastState = LOW;
#ifdef USE_COUNTER_BUTTONS
      readCounterButtons(); //We need two more interrupt pins to get this working!
#endif
    }
#else
#ifdef USE_COUNTER_BUTTONS
    readCounterButtons(); //We need two more interrupt pins to get this working!
#endif
    request(CHILD_ID_THR, V_TEXT, 0);
    request(CHILD_ID_PC, V_TEXT, 0);
    readSensorData();
#endif
#ifdef USE_BATTERY
    smartSleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), RISING, SLEEP_TIME); //sleep function only in battery mode needed
    readSensorData();
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Counter: ");
    oled.println(peopleCount);
#endif

    while (motion.checkMotion() != LOW)
    {

      readSensorData();
    }
#endif
  }
  else
  {
    lastState = HIGH;
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Counter: ");
    oled.println(peopleCount);
#endif
    while (motion.checkMotion() != LOW)
    {
      readSensorData();
    }
  }
}

// void readSensorData()
// {
//   int starttime = millis();
//   int endtime = starttime;
//   int inout = -1;                        //if inout== 0 -> out; if inout == 1 --> in; THIS STATE WILL BE SENT!
//   while ((endtime - starttime) <= LTIME) // do this loop for up to 5000mS
//   {
//     inout = -1;
// #if defined USE_SHARP_IR
//     // turn both sensors on
//     digitalWrite(ROOM_ENABLE, HIGH);
//     wait(1);
//     digitalWrite(CORRIDOR_ENABLE, HIGH);
//     wait(5);
//     irrVal = analogRead(ANALOG_IR_SENSORR);
//     wait(10);
//     ircVal = analogRead(ANALOG_IR_SENSORC);
// #elif defined USE_VL53L0X
//     wait(10);
//     irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//     wait(10);
//     ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
// #endif

// #ifdef MY_DEBUG
//     Serial.print("IRR:");
//     Serial.println(irrVal);
//     Serial.print("IRC:");
//     Serial.println(ircVal);
// #endif

// #if defined USE_SHARP_IR
//     if (irrVal > threshold && ircVal < threshold && inout != 1)
//     {
//       int startR = millis();
//       int endR = startR;
//       while ((endR - startR) <= MTIME)
//       {
//         irrVal = analogRead(ANALOG_IR_SENSORR);
//         wait(10);
//         ircVal = analogRead(ANALOG_IR_SENSORC);
//         if (ircVal > threshold && irrVal > threshold)
//         {
// #ifdef MY_DEBUG
//           Serial.print("In Loop IRR: ");
//           Serial.println(irrVal);
//           Serial.print("In Loop IRC: ");
//           Serial.println(ircVal);
//           Serial.print("Delay Time: ");
//           Serial.println(MTIME - (endR - startR));
// #endif
//           while (irrVal > threshold || ircVal > threshold)
//           {
//             irrVal = analogRead(ANALOG_IR_SENSORR);
//             wait(10);
//             ircVal = analogRead(ANALOG_IR_SENSORC);
//             if (ircVal > threshold && irrVal < threshold)
//             {
//               // turn both sensors off
//               digitalWrite(ROOM_ENABLE, LOW);
//               wait(1);
//               digitalWrite(CORRIDOR_ENABLE, LOW);
//               inout = 0;
//               sendCounter(inout);
//               break;
//             }
//             wait(11);
//           }

//           if (inout == 0)
//           {
//             wait(150);
//             ircVal = 0;
//             endR = millis();
//             starttime = millis();
//             endtime = starttime;
//             break;
//           }
//         }
//         else
//         {
//           wait(1);
//           endR = millis();
//         }
//       }
//     }
//     else
//     {
//       endtime = millis();
//     }
//     if (ircVal > threshold && irrVal < threshold && inout != 0)
//     {
//       int startC = millis();
//       int endC = startC;
//       while ((endC - startC) <= MTIME)
//       {
//         ircVal = analogRead(ANALOG_IR_SENSORR);
//         wait(10);
//         irrVal = analogRead(ANALOG_IR_SENSORC);
//         if (irrVal > threshold && ircVal > threshold)
//         {
// #ifdef MY_DEBUG
//           Serial.print("In Loop IRC: ");
//           Serial.println(ircVal);
//           Serial.print("In Loop IRR: ");
//           Serial.println(irrVal);
//           Serial.print("Delay Time: ");
//           Serial.println(MTIME - (endC - startC));
// #endif
//           while (irrVal > threshold || ircVal > threshold)
//           {
//             irrVal = analogRead(ANALOG_IR_SENSORR);
//             wait(10);
//             ircVal = analogRead(ANALOG_IR_SENSORC);
//             if (irrVal > threshold && ircVal < threshold)
//             {
//               // turn both sensors off
//               digitalWrite(ROOM_ENABLE, LOW);
//               wait(1);
//               digitalWrite(CORRIDOR_ENABLE, LOW);

//               inout = 1;
//               sendCounter(inout);
//               break;
//             }
//             wait(11);
//           }

//           if (inout == 1)
//           {
//             wait(150);
//             irrVal = 0;
//             endC = millis();
//             starttime = millis();
//             endtime = starttime;
//             break;
//           }
//         }
//         else
//         {
//           wait(11);
//           endC = millis();
//         }
//       }
//     }
//     else
//     {
//       endtime = millis();
//     }
//     wait(10);
// #elif defined USE_VL53L0X
//     if (irrVal < threshold && ircVal > threshold && inout != 1)
//     {
//       int startR = millis();
//       int endR = startR;
//       while ((endR - startR) <= MTIME)
//       {
//         irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//         wait(10);
//         ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
//         if (ircVal < threshold && irrVal < threshold)
//         {
// #ifdef MY_DEBUG
//           Serial.print("In Loop IRR: ");
//           Serial.println(irrVal);
//           Serial.print("In Loop IRC: ");
//           Serial.println(ircVal);
//           Serial.print("Delay Time: ");
//           Serial.println(MTIME - (endR - startR));
// #endif
//           while (irrVal < threshold || ircVal < threshold)
//           {
//             irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//             wait(10);
//             ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
//             if (ircVal < threshold && irrVal > threshold)
//             {
//               inout = 0;
//               sendCounter(inout);
//               break;
//             }
//             wait(11);
//           }

//           if (inout == 0)
//           {
//             wait(150);
//             ircVal = 0;
//             endR = millis();
//             starttime = millis();
//             endtime = starttime;
//             break;
//           }
//         }
//         else
//         {
//           wait(10);
//           endR = millis();
//         }
//       }
//     }
//     else
//     {
//       endtime = millis();
//     }
//     if (ircVal < threshold && irrVal > threshold && inout != 0)
//     {
//       int startC = millis();
//       int endC = startC;
//       while ((endC - startC) <= MTIME)
//       {
//         irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//         wait(10);
//         ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
//         if (irrVal < threshold && ircVal < threshold)
//         {
// #ifdef MY_DEBUG
//           Serial.print("In Loop IRC: ");
//           Serial.println(ircVal);
//           Serial.print("In Loop IRR: ");
//           Serial.println(irrVal);
//           Serial.print("Delay Time: ");
//           Serial.println(MTIME - (endC - startC));
// #endif
//           while (irrVal < threshold || ircVal < threshold)
//           {
//             irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//             wait(10);
//             ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();
//             if (irrVal < threshold && ircVal > threshold)
//             {
//               inout = 1;
//               sendCounter(inout);
//               break;
//             }
//             wait(11);
//           }

//           if (inout == 1)
//           {
//             wait(150);
//             irrVal = 0;
//             endC = millis();
//             starttime = millis();
//             endtime = starttime;
//             break;
//           }
//         }
//         else
//         {
//           wait(10);
//           endC = millis();
//         }
//       }
//     }
//     else
//     {
//       endtime = millis();
//     }
//     wait(10);
// #endif
//   }
// }
#ifdef USE_COUNTER_BUTTONS
void readCounterButtons()
{
  if (digitalRead(INCREASE_BUTTON) == HIGH)
  {
    sendCounter(1);
  }
  else if (digitalRead(DECREASE_BUTTON) == HIGH)
  {
    sendCounter(0);
  }
}
#endif

// void sendCounter(int inout)
// {
//   if (inout == 1)
//   {
//     peopleCount++;
//     send(msg.set(inout));
//     wait(30);
//     send(pcMsg.set(peopleCount));
//   }
//   else if (inout == 0)
//   {
//     if (peopleCount > 0)
//     {
//       peopleCount--;
//     }
//     if (peopleCount == 0)
//     {
//       send(msg.set(inout));
//     }
//     wait(30);
//     send(pcMsg.set(peopleCount));
//   }

// #ifdef USE_BATTERY
//   float voltage = battery.checkBatteryLevel();
//   send(voltage_msg.set(voltage, 3)); // redVcc returns millivolts. Set wants volts and how many decimals (3 in our case)
//   sendBatteryLevel(round((voltage - BATTERY_ZERO) * 100.0 / (BATTERY_FULL - BATTERY_ZERO)));
// #endif
// #ifdef USE_OLED
//   oled.clear();
//   oled.setCursor(5, 0);
//   oled.setTextSize(2, 1);
//   oled.print("Counter: ");
//   oled.println(peopleCount);
// #endif
// }

// int calculateStandardDeviation(int irValues[])
// {
//   auto sumOfValues = 0;
//   auto arrLength = 0;
//   for (int i = 0; i < 30; i++)
//   {
//     if (irValues[i] != 0)
//     {
//       sumOfValues += irValues[i];
//       arrLength++;
//     }
//   }

//   auto meanValue = sumOfValues / arrLength;

//   auto standardDeviation = 0;
//   for (int i = 0; i < arrLength; ++i)
//   {
//     standardDeviation += pow(irValues[i] - meanValue, 2);
//   }
//   standardDeviation /= arrLength;

//   standardDeviation = sqrt(standardDeviation);

//   return standardDeviation;
// }

// int calibration()
// {
//   int irValues[30] = {};
// #ifdef USE_OLED
//   oled.clear();
//   oled.setCursor(0, 5);
//   oled.setTextSize(1, 1);
//   oled.println("### Calibrate IR ###");
// #endif

// #if defined(USE_SHARP_IR)
//   digitalWrite(CORRIDOR_ENABLE, HIGH);
//   digitalWrite(ROOM_ENABLE, HIGH);
//   delay(100);
//   auto max = 0;
// #elif defined USE_VL53L0X
//   auto min = 0;
// #endif
//   auto n = 0;
//   for (int m = 0; m < CALIBRATION_VAL; m++)
//   {
// #if defined(USE_SHARP_IR)
//     wait(10);
//     irrVal = analogRead(ANALOG_IR_SENSORR);
//     wait(10);
//     ircVal = analogRead(ANALOG_IR_SENSORC);

//     //calculate the max without jumps for the room sensor
//     if (((irrVal > max) && ((irrVal - max) < THRESHOLD_X)) || ((irrVal - max) == irrVal))
//     {
//       Serial.println(irrVal);
//       max = irrVal;
//       if (n < 30)
//       {
//         irValues[n] = max;
//         n++;
//       }
//     }
//     //calculate the max without jumps for the corridor sensor
//     if (((ircVal > max) && ((ircVal - max) < THRESHOLD_X)) || ((ircVal - max) == ircVal))
//     {
//       Serial.println(ircVal);
//       max = ircVal;
//       if (n < 30)
//       {
//         irValues[n] = max;
//         n++;
//       }
//     }
// #elif defined(USE_VL53L0X)
//     wait(10);
//     irrVal = ROOM_SENSOR.readRangeContinuousMillimeters();
//     wait(10);
//     ircVal = CORRIDOR_SENSOR.readRangeContinuousMillimeters();

//     //calculate the max without jumps for the room sensor
//     if ((irrVal < min) || ((irrVal - min) == irrVal))
//     {
//       Serial.println(irrVal);
//       min = irrVal;
//       if (n < 30)
//       {
//         irValues[n] = min;
//         n++;
//       }
//     }
//     //calculate the max without jumps for the corridor sensor
//     if ((ircVal < min) || ((ircVal - min) == ircVal))
//     {
//       Serial.println(ircVal);
//       min = ircVal;
//       if (n < 30)
//       {
//         irValues[n] = min;
//         n++;
//       }
//     }
// #endif
//   }

// #if defined USE_SHARP_IR
//   // shutdown both sensors
//   digitalWrite(CORRIDOR_ENABLE, LOW);
//   digitalWrite(ROOM_ENABLE, LOW);
//   auto sd = calculateStandardDeviation(irValues);
//   threshold = max + sd;
// #elif defined USE_VL53L0X
//   auto sd = calculateStandardDeviation(irValues);
//   threshold = min - sd;
// #endif

//   // Serial.print("standard deviation: " + threshold);
//   // threshold = max + THRESHOLD_X;
// #ifdef USE_OLED
//   oled.setCursor(15, 0);
//   oled.print("Threshold: ");
//   oled.println(threshold);
//   wait(2000);
//   oled.clear();
//   oled.setCursor(10, 0);
//   oled.setTextSize(1, 1);
//   oled.println("Calibration done!");
//   wait(2000);
// #endif
//   Serial.print("standard deviation: ");
//   Serial.println(sd);
//   Serial.print("New threshold is: ");
//   Serial.println(threshold);
//   send(thrMsg.set(threshold));
//   Serial.println("#### calibration done ####");
//   return threshold;
// }

/*
  - Not sure if this function ever will be used... But good idea to have this as backup
  - Tests will be show more about RAM problems
  - commented out as it is not used yet!
*/
// int freeRam()
// {
//   extern int __heap_start, *__brkval;
//   int v;
//   return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
// }
