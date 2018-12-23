/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/

#include <Configuration.h>
#include <MySensors.h>    // include the MySensors library
#include <Arduino.h>      //need to be included, cause the file is moved to a .cpp file
#include <MotionSensor.h> //MotionSensorLib
#include <SensorReader.h>
#include <Communication.h>
#include <Calibration.h>

// battery setup
#ifdef USE_BATTERY
#include <BatteryMeter.h> //Include and Set Up BatteryMeter Library
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
extern int peopleCount;
VL53L0X CORRIDOR_SENSOR;
VL53L0X ROOM_SENSOR;
void readCounterButtons();

void setup()
{
#ifdef USE_OLED
  oled.init();
  oled.sendCommand(BRIGHTNESS_CTRL);
  oled.sendCommand(BRIGHTNESS);
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

  //Motion Sensor
  pinMode(DIGITAL_INPUT_SENSOR, INPUT); // declare motionsensor as input
#if defined(USE_SHARP_IR) && !defined(USE_VL53L0X)
  //Corridor Sensor Enable PIN
  pinMode(CORRIDOR_ENABLE, OUTPUT);

  //Room Sensor Voltage Enable PIN
  pinMode(ROOM_ENABLE, OUTPUT);
#endif
#if !defined(USE_SHARP_IR) && defined(USE_VL53L0X)

  pinMode(ROOM_XSHUT, OUTPUT);
  pinMode(CORRIDOR_XSHUT, OUTPUT);
  Wire.begin();

  pinMode(ROOM_XSHUT, INPUT);
  wait(10);
  ROOM_SENSOR.setAddress(ROOM_SENSOR_newAddress);
  pinMode(CORRIDOR_XSHUT, INPUT);
  wait(10);
  CORRIDOR_SENSOR.setAddress(CORRIDOR_SENSOR_newAddress);
  ROOM_SENSOR.init();
  CORRIDOR_SENSOR.init();
  ROOM_SENSOR.setTimeout(500);
  CORRIDOR_SENSOR.setTimeout(500);

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
  ROOM_SENSOR.startContinuous();
  CORRIDOR_SENSOR.startContinuous();
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

  calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
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
      ROOM_SENSOR.startContinuous();
      CORRIDOR_SENSOR.startContinuous();
      wait(30);
      calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
    }

    if (message.sensor == CHILD_ID_PC)
    {
      wait(30);
      Serial.println(message.getInt());
      peopleCount = message.getInt();
      // send(pcMsg.set(peopleCount));
    }
  }
}
int lastState = LOW;
int newState = LOW;

void loop()
{
  if (ROOM_SENSOR.timeoutOccurred() || CORRIDOR_SENSOR.timeoutOccurred())
  {
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Timeout occured!");
#endif
    reportToController(0, 65535);
    Serial.println("Timeout occured. Reinitialize Sensors");
    ROOM_SENSOR.init();
    CORRIDOR_SENSOR.init();
    ROOM_SENSOR.setTimeout(500);
    CORRIDOR_SENSOR.setTimeout(500);
    ROOM_SENSOR.startContinuous();
    CORRIDOR_SENSOR.startContinuous();
    calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
  }

  //   // Sleep until interrupt comes in on motion sensor. Send never an update
  if (motion.checkMotion() == LOW)
  {
#ifdef USE_OLED
    oled.clear();
#endif
#ifdef USE_ENEGERY_SAVING
    if (lastState == HIGH)
    {
      readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
#if defined USE_SHARP_IR
      digitalWrite(ROOM_ENABLE, LOW);
      wait(1);
      digitalWrite(CORRIDOR_ENABLE, LOW);
#elif defined USE_VL53L0X || defined USE_VL53L1X
#ifdef MY_DEBUG
      Serial.println("Shutting down sensors");
#endif
      ROOM_SENSOR.stopContinuous();
      CORRIDOR_SENSOR.stopContinuous();
#endif
      // wait(30);
      // request(CHILD_ID_THR, V_TEXT, 0);
      // wait(30);
      // request(CHILD_ID_PC, V_TEXT, 0);
      lastState = LOW;
#ifdef USE_COUNTER_BUTTONS
      readCounterButtons(); //We need two more interrupt pins to get this working!
#endif
    }
#else
#ifdef USE_COUNTER_BUTTONS
    readCounterButtons(); //We need two more interrupt pins to get this working!
#endif
    // request(CHILD_ID_THR, V_TEXT, 0);
    // wait(30);
    // request(CHILD_ID_PC, V_TEXT, 0);
    readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
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

      readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
    }
#endif
  }
  else
  {
    if (lastState == LOW)
    {
#ifdef USE_ENEGERY_SAVING
#ifdef MY_DEBUG
      Serial.println("Starting continuous mode again");
#endif
      ROOM_SENSOR.startContinuous();
      CORRIDOR_SENSOR.startContinuous();
      wait(10);
#endif
    }
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
      readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
    }
  }
}

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
