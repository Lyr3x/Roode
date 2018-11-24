/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmai.com
License: GPLv3
*/

#include <Configuration.h>
#include <MySensors.h>    // include the MySensors library
#include <Arduino.h>      //need to be included, cause the file is moved to a .cpp file
#include <MotionSensor.h> //MotionSensorLib
#include <BatteryMeter.h> //Include and Set Up BatteryMeter Library

// battery setup
#ifdef USE_BATTERY
BatteryMeter battery(BATTERY_METER_PIN);            //BatteryMeter instance
MyMessage voltage_msg(CHILD_ID_BATTERY, V_VOLTAGE); //MySensors battery voltage message instance
#endif

/* MySensors Message types and default settings */
unsigned long SLEEP_TIME = 0; //sleep forever
#define CHILD_ID_R 0
#define CHILD_ID_PC 1
#define CHILD_ID_THR 3
MyMessage msg(CHILD_ID_R, V_STATUS);    //room on/off child
MyMessage pcMsg(CHILD_ID_PC, V_TEXT);   //people counter child
MyMessage thrMsg(CHILD_ID_THR, V_TEXT); //Threshold child

/* Motion Sensor setup*/
MotionSensor motion(DIGITAL_INPUT_SENSOR);

// some needed var declarations
int irrVal = 0;      //analog value store for the room sensor
int ircVal = 0;      //analog value store for the corridor sensor
int threshold = 180; //if CALIBRATION is not defined, this threshold is used (okay for a 80cm doorway using reflection foil or white paper)
int peopleCount = 0; //default state: nobody is inside the room

// function prototypes
void irSensor();
int calibration();
void sendCounter(int inout);
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

  //Corridor Sensor Enable PIN
  pinMode(IR_D_C, OUTPUT);

  //Room Sensor Voltage Enable PIN
  pinMode(IR_D_R, OUTPUT);

  //Motion Sensor
  pinMode(DIGITAL_INPUT_SENSOR, INPUT); // declare motionsensor as input

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
  wait(2000);
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
    if (lastState == HIGH)
    {
      irSensor(); //One more tracking phase before do some powersaving
      digitalWrite(IR_D_R, LOW);
      wait(1);
      digitalWrite(IR_D_C, LOW);
      request(CHILD_ID_THR, V_TEXT, 0);
      request(CHILD_ID_PC, V_TEXT, 0);
      lastState = LOW;
      readCounterButtons(); //We need two more interrupt pins to get this working!
    }
#ifdef USE_BATTERY
    smartSleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), RISING, SLEEP_TIME); //sleep function only in battery mode needed
    irSensor();
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Counter: ");
    oled.println(peopleCount);
#endif

    while (motion.checkMotion() != LOW)
    {

      irSensor();
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
      irSensor();
    }
  }
}

void irSensor()
{
  int starttime = millis();
  int endtime = starttime;
  int inout = -1;                        //if inout== 0 -> out; if inout == 1 --> in; THIS STATE WILL BE SENT!
  while ((endtime - starttime) <= LTIME) // do this loop for up to 5000mS
  {
    // turn both sensors on
    digitalWrite(IR_D_R, HIGH);
    wait(1);
    digitalWrite(IR_D_C, HIGH);
    wait(5);
    inout = -1;

    irrVal = analogRead(ANALOG_IR_SENSORR);
    wait(10);
    ircVal = analogRead(ANALOG_IR_SENSORC);

#ifdef MY_DEBUG
    Serial.print("IRR:");
    Serial.println(irrVal);
    Serial.print("IRC:");
    Serial.println(ircVal);
#endif
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
              digitalWrite(IR_D_R, LOW);
              wait(1);
              digitalWrite(IR_D_C, LOW);
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
        ircVal = analogRead(ANALOG_IR_SENSORC);
        wait(10);
        irrVal = analogRead(ANALOG_IR_SENSORR);
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
              digitalWrite(IR_D_R, LOW);
              wait(1);
              digitalWrite(IR_D_C, LOW);
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
    wait(11);
  }
}

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

void sendCounter(int inout)
{
  if (inout == 1)
  {
    peopleCount++;
    send(msg.set(inout));
    wait(30);
    send(pcMsg.set(peopleCount));
  }
  else if (inout == 0)
  {
    if (peopleCount > 0)
    {
      peopleCount--;
    }
    if (peopleCount == 0)
    {
      send(msg.set(inout));
    }
    wait(30);
    send(pcMsg.set(peopleCount));
  }

#ifdef USE_BATTERY
  float voltage = battery.checkBatteryLevel();
  send(voltage_msg.set(voltage, 3)); // redVcc returns millivolts. Set wants volts and how many decimals (3 in our case)
  sendBatteryLevel(round((voltage - BATTERY_ZERO) * 100.0 / (BATTERY_FULL - BATTERY_ZERO)));
#endif
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(5, 0);
  oled.setTextSize(2, 1);
  oled.print("Counter: ");
  oled.println(peopleCount);
#endif
}

int calculateStandardDeviation(int irValues[])
{
  auto sumOfValues = 0;
  auto arrLength = 0;
  for (int i = 0; i < 62; i++)
  {
    if (irValues[i] != 0)
    {
      sumOfValues += irValues[i];
      arrLength++;
    }
  }
  auto meanValue = sumOfValues / arrLength;
  auto standardDeviation = 0;
  for (int i = 0; i < arrLength; i++)
  {
    standardDeviation += (irValues[i] - meanValue) * (irValues[i] - meanValue);
  }
  standardDeviation /= arrLength;
  standardDeviation = sqrt(standardDeviation);
  return standardDeviation;
}

int calibration()
{
  int irValues[62];
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(0, 5);
  oled.setTextSize(1, 1);
  oled.println("### Calibrate IR ###");
#endif
  digitalWrite(IR_D_C, HIGH);
  digitalWrite(IR_D_R, HIGH);
  delay(100);
  // int distances[CALIBRATION_VAL];
  auto max = 0;
  auto n = 0;
  for (int m = 0; m < CALIBRATION_VAL; m = m + 2)
  {
    delay(8);
    irrVal = analogRead(ANALOG_IR_SENSORR);
    delay(8);
    ircVal = analogRead(ANALOG_IR_SENSORC);

    //calculate the max without jumps for the room sensor
    if (((irrVal > max) && ((irrVal - max) < THRESHOLD_X)) || ((irrVal - max) == irrVal))
    {
      Serial.println(irrVal);
      max = irrVal;
      irValues[n] = max;
      n++;
    }
    //calculate the max without jumps for the corridor sensor
    if (((ircVal > max) && ((ircVal - max) < THRESHOLD_X)) || ((ircVal - max) == ircVal))
    {
      Serial.println(ircVal);
      max = ircVal;
      irValues[n] = max;
      n++;
    }
  }

  // shutdown both sensors
  digitalWrite(IR_D_C, LOW);
  digitalWrite(IR_D_R, LOW);
  threshold = max + calculateStandardDeviation(irValues);
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
  Serial.print("New threshold is: ");
  Serial.println(threshold);
  send(thrMsg.set(threshold));
  Serial.println("#### calibration done ####");
  return threshold;
}

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
