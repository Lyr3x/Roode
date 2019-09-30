/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/
#include <Config.h>
#include <Arduino.h> //need to be included, cause the file is moved to a .cpp file

#include <Wire.h>

#ifdef USE_MQTT
#include <MQTTTransmitter.h>
MQTTTransmitter transmitter;
const char *topic_Domoticz_IN = "domoticz/in";
const char *topic_Domoticz_OUT = "domoticz/out";
#endif

#ifdef USE_MYSENSORS
#include <MySensors.h> // include the MySensors library
#include <MySensorsTransmitter.h>
MySensorsTransmitter transmitter;
#endif

#include <OptionChecker.h>
#include <MotionSensor.h> //MotionSensorLib
#include <VL53L0XSensor.h>
#include <PeopleCounter.h>

// battery setup
#ifdef USE_BATTERY
#include <BatteryMeter.h>                           //Include and Set Up BatteryMeter Library
BatteryMeter battery(BATTERY_METER_PIN);            //BatteryMeter instance
MyMessage voltage_msg(CHILD_ID_BATTERY, V_VOLTAGE); //MySensors battery voltage message instance
#endif

VL53L0XSensor ROOM_SENSOR(ROOM_XSHUT, ROOM_SENSOR_newAddress);
VL53L0XSensor CORRIDOR_SENSOR(CORRIDOR_XSHUT, CORRIDOR_SENSOR_newAddress);

void manageTimeout();        //move to sensor
void updateDisplayCounter(); //move to display module
void sensorCalibration();    //move to Calibration module
void setup()
{
#ifdef USE_MQTT
  Wire.begin(D6, D5);
  Serial.begin(115200);
  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(transmitter.ssid);

  // connect to WiFi Access Point
  // ESP.wdtDisable(); //Disable soft watch dog
  WiFi.mode(WIFI_STA);
  WiFi.begin(transmitter.ssid, transmitter.password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection to the main WiFi Failed!");
    delay(2000);
    if (transmitter.WiFi_AP == 1)
    {
      transmitter.WiFi_AP = 2;
      Serial.println("Trying to connect to the alternate WiFi...");
      WiFi.begin(transmitter.ssid2, transmitter.password2);
    }
    else
    {
      transmitter.WiFi_AP = 1;
      Serial.println("Trying to connect to the main WiFi...");
      WiFi.begin(transmitter.ssid, transmitter.password);
    }
  }

  //MQTT
  void callback(char *topic, byte *payload, unsigned int length);
  client.setServer(transmitter.mqtt_server, 1883);
  client.setCallback(callback);

  // say we are now ready and give configuration items
  Serial.println("Ready");
  Serial.print("Connected to ");
  if (transmitter.WiFi_AP == 1)
    Serial.println(transmitter.ssid);
  else
    Serial.println(transmitter.ssid2);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (!client.connected())
  { // MQTT connection
    transmitter.reconnect();
  }
#endif
#ifdef USE_MYSENSORS
  Wire.begin();
#endif
#ifdef USE_OLED
  oled.begin(&Adafruit128x32, OLED_I2C);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.setContrast(BRIGHTNESS);
  oled.setCursor(10, 0);
  oled.print("RooDe: ");
  oled.print(ROODE_VERSION);
  oled.print("\n");
#ifdef USE_MYSENSORS
  oled.print("MySensors: ");
  oled.println(MYSENSORS_LIBRARY_VERSION);
#endif
#ifdef USE_MQTT
  oled.println("PubSubClient: v2.7 ");

#endif
  delay(2000);
  oled.clear();
#endif

  Serial.println(F("##### RooDe Presence Detection System #####"));

  //Motion Sensor
  pinMode(DIGITAL_INPUT_SENSOR, INPUT); // declare motionsensor as input

  // Initialize VL53LXX sensors
  ROOM_SENSOR.init();
  delay(10);
  CORRIDOR_SENSOR.init();

  ROOM_SENSOR.startContinuous();
  CORRIDOR_SENSOR.startContinuous();

#ifdef CALIBRATION

  motion.Setup(MOTION_INIT_TIME);
  // Serial.println("#### motion sensor initialized ####");

  Serial.println(F("#### Sensor calibration starting ####"));
#ifdef USE_OLED
  oled.clear();
  oled.println("Sensor calibration");
#endif
  sensorCalibration();
#if defined(USE_OLED)
  oled.clear();
  oled.print("Right: ");
  oled.println(ROOM_SENSOR.getThreshold());
  oled.print("Left: ");
  oled.println(CORRIDOR_SENSOR.getThreshold());
  delay(1500);
#endif
#endif

  Serial.println(F("#### Setting the PresenceCounter and Status to OUT (0) ####"));
#ifdef USE_MQTT
  if (!client.connected())
  { // MQTT connection
    transmitter.reconnect();
  }
#endif
  transmitter.transmit(transmitter.devices.room_switch, 0, "Off");
  delay(10);
  transmitter.transmit(transmitter.devices.peoplecounter, 0);
  peopleCount = 0;
  // #if defined(USE_OLED)
  //   peopleCount = 0;
  //   updateDisplayCounter();
  // #endif
} // end of setup()
#ifdef USE_MYSENSORS
void presentation()
{
  transmitter.presentation();
}
#endif

int lastState = LOW;
void loop()
{
#ifdef USE_MQTT
  if (!client.connected())
  { // MQTT connection
    transmitter.reconnect();
  }
#endif

  if (ROOM_SENSOR.timeoutOccurred() || CORRIDOR_SENSOR.timeoutOccurred())
  {
    manageTimeout();
  }

  //   // Sleep until interrupt comes in on motion sensor. Send never an update
  if (motion.checkMotion() == LOW)
  {
#ifdef MY_DEBUG
    Serial.println("1. Motion sensor is off");
#endif
#ifdef USE_OLED
    oled.clear();
#endif
#ifdef USE_ENEGERY_SAVING
    if (lastState == HIGH)
    {

#ifdef MY_DEBUG
      Serial.println("2. Motion sensor is off. Last readloop");
#endif
      peoplecounting(ROOM_SENSOR, CORRIDOR_SENSOR, transmitter);

#ifdef MY_DEBUG
      Serial.println("3. Shutting down sensors");
#endif
      lastState = LOW;
      ROOM_SENSOR.stopContinuous();
      CORRIDOR_SENSOR.stopContinuous();
    }
#else
    peoplecounting(ROOM_SENSOR, CORRIDOR_SENSOR, transmitter);
#endif
#ifdef USE_BATTERY
    smartSleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), RISING, SLEEP_TIME); //sleep function only in battery mode needed
    peoplecounting(ROOM_SENSOR, CORRIDOR_SENSOR, transmitter);
#if defined(USE_OLED) || defined(USE_OLED)
    updateDisplayCounter();
#endif

    while (motion.checkMotion() != LOW)
    {
      yield();
#ifdef MY_DEBUG
      Serial.println("4. Motion sensor is on. Start counting");
#endif
      peoplecounting(ROOM_SENSOR, CORRIDOR_SENSOR, transmitter);
    }
#endif
  }
  else //Motion HIGH
  {
    if (lastState == LOW)
    {
#ifdef USE_OLED
      updateDisplayCounter();
#endif
#ifdef USE_ENEGERY_SAVING
#ifdef MY_DEBUG
      Serial.println("5. Starting continuous mode again");
#endif
#ifdef MY_DEBUG
      Serial.println("6. Start Sensors");
#endif
      ROOM_SENSOR.startContinuous();
      CORRIDOR_SENSOR.startContinuous();
#endif
    }
    lastState = HIGH;
    while (motion.checkMotion() != LOW)
    {
#ifdef MY_DEBUG
      Serial.println("7. Motion sensor is on. Start counting");
#endif
      peoplecounting(ROOM_SENSOR, CORRIDOR_SENSOR, transmitter);
    }
  }
  delay(10);
#ifdef USE_MQTT
  client.loop();
#endif
}

inline void updateDisplayCounter()
{
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(5, 0);
  oled.set2X();
  oled.print("Inside: ");
  oled.println(peopleCount);
#endif
}

inline void manageTimeout()
{
#ifdef USE_OLED
  oled.clear();
  oled.setCursor(5, 0);
  oled.set2X();
  oled.print("Timeout occured!");
#endif
  // reportToController(65535);
  Serial.println("Timeout occured. Restart the System");
  sensorCalibration();
}

inline void sensorCalibration()
{
  Serial.println("#### calibrate the ir sensors ####");
  int room_threhsold = ROOM_SENSOR.calibration();
  int corridor_threhsold = CORRIDOR_SENSOR.calibration();
  char buf[40];
  sprintf(buf, "Room: %d, Corridor: %d", room_threhsold, corridor_threhsold);

  transmitter.transmit(transmitter.devices.threshold, 0, buf);
}
#ifdef USE_MQTT
// !! Needs to be implemented !!

void callback(char *topic, byte *payload, unsigned int length)
{ // ****************

  DynamicJsonDocument root(MQTT_MAX_PACKET_SIZE);
  String messageReceived = "";

  // Affiche le topic entrant - display incoming Topic
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));

  // decode payload message
  for (unsigned int i = 0; i < length; i++)
  {
    messageReceived += ((char)payload[i]);
  }
  // display incoming message
  Serial.print(messageReceived);

  // if domoticz message
  if (strcmp(topic, topic_Domoticz_OUT) == 0)
  {
    //JsonObject& root = jsonBuffer.parseObject(messageReceived);
    DeserializationError error = deserializeJson(root, messageReceived);
    if (error)

      return;

    const char *idxChar = root["idx"];
    String idx = String(idxChar);
    /*
        if ( idx == LIGHT_SWITCH_IDX[0] ) {      
           const char* cmde = root["nvalue"];
           if( strcmp(cmde, "0") == 0 ) {  // 0 means we have to switch OFF the lamps
                if( LIGHT_ACTIVE[0] == "On" ) { digitalWrite(Relay1, LOW); LIGHT_ACTIVE[0] = "Off"; } 
           } else if( LIGHT_ACTIVE[0] == "Off" ) { digitalWrite(Relay1, HIGH); LIGHT_ACTIVE[0] = "On"; }           
           Serial.print("Lighting "); Serial.print(LIGHTING[0]); Serial.print(" is now : "); Serial.println(LIGHT_ACTIVE[0]);
        }  // if ( idx == LIGHT_SWITCH_IDX[0] ) {

        if ( idx == LIGHT_SWITCH_IDX[1] ) {      
           const char* cmde = root["nvalue"];
           if( strcmp(cmde, "0") == 0 ) { 
                if( LIGHT_ACTIVE[1] == "On" ) { digitalWrite(Relay2, LOW); LIGHT_ACTIVE[1] = "Off"; } 
           } else if( LIGHT_ACTIVE[1] == "Off" ) { digitalWrite(Relay2, HIGH); LIGHT_ACTIVE[1] = "On"; }    
           Serial.print("Lighting "); Serial.print(LIGHTING[1]); Serial.print(" is now : "); Serial.println(LIGHT_ACTIVE[1]);
        }  // if ( idx == LIGHT_SWITCH_IDX[0] ) {
            */
  } // if domoticz message

  delay(15);
} // void callback(char* to   ****************

#endif

#ifdef USE_MYSENSORS
// MySensors receive function
void receive(const MyMessage &message)
{
  int result = transmitter.receive(message);
  if (result == -1)
  {
    Serial.println(F("Sensor calibration"));
    sensorCalibration();
  }
  else
  {
    peopleCount = result;
  }
}

#endif
