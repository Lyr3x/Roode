/*  RooDe - Room Presence Detection
Author: Kai Bepperling, kai.bepperling@gmail.com
License: GPLv3
*/
#include <Arduino.h>      //need to be included, cause the file is moved to a .cpp file
#include <Configuration.h>
#include <Wire.h>

#ifdef USE_MYSENSORS
  #include <MySensors.h> // include the MySensors library
  #include <MySensorsTransmitter.h>
  MySensorsTransmitter transmitter;
#endif

#ifdef USE_MQTT
#include <ESP8266WiFi.h>
#include <MQTTTransmitter.h>
MQTTTransmitter transmitter;
const char *topic_Domoticz_IN = "domoticz/in";   //$$
const char *topic_Domoticz_OUT = "domoticz/out"; //$$
#endif

#include <OptionChecker.h>
#include <MotionSensor.h> //MotionSensorLib
#include <Calibration.h>
#include <VL53L0XSensor.h>
//USE_MQTT
// battery setup
#ifdef USE_BATTERY
#include <BatteryMeter.h>                           //Include and Set Up BatteryMeter Library
BatteryMeter battery(BATTERY_METER_PIN);            //BatteryMeter instance
MyMessage voltage_msg(CHILD_ID_BATTERY, V_VOLTAGE); //MySensors battery voltage message instance
#endif

// extern uint8_t peopleCount;
uint8_t peopleCount;
VL53L0XSensor ROOM_SENSOR(ROOM_XSHUT, ROOM_SENSOR_newAddress);
VL53L0XSensor CORRIDOR_SENSOR(CORRIDOR_XSHUT, CORRIDOR_SENSOR_newAddress);
// VL53L0XSensor ROOM_SENSOR = new VL53L0XSensor(ROOM_XSHUT, ROOM_SENSOR_newAddress);
// VL53L0XSensor CORRIDOR_SENSOR = new VL53L0XSensor(CORRIDOR_XSHUT, CORRIDOR_SENSOR_newAddress);
/*#ifdef USE_VL53L0X
VL53L0X CORRIDOR_SENSOR;
VL53L0X ROOM_SENSOR;

#elif defined USE_VL53L1X
VL53L1X CORRIDOR_SENSOR_pololu;
VL53L1X ROOM_SENSOR_pololu;
VL53L1XWrap ROOM_SENSOR(ROOM_SENSOR_pololu);
VL53L1XWrap CORRIDOR_SENSOR(CORRIDOR_SENSOR_pololu);
#endif
void readCounterButtons();
*/

void setup()
{
#ifdef USE_MQTT
  Wire.begin(D6, D5);
  // transmitter.init();
  Serial.begin(115200);
  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(transmitter.ssid);

  // connect to WiFi Access Point
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
#elif defined USE_MYSENSORS
  Wire.begin();
#endif

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
  delay(2000);
#endif

  Serial.println("##### RooDe Presence Detection System #####");

  //Motion Sensor
  pinMode(DIGITAL_INPUT_SENSOR, INPUT); // declare motionsensor as input

  //DistanceSensors
  ROOM_SENSOR.init();
  CORRIDOR_SENSOR.init();

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
  ROOM_SENSOR.calibration();
  CORRIDOR_SENSOR.calibration();
  // calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
#endif

  Serial.println("#### Setting the PresenceCounter and Status to OUT (0) ####");

  transmitter.transmit(transmitter.devices.room_switch, 0);
  transmitter.transmit(transmitter.devices.peoplecounter, 0);

#ifdef USE_OLED
  oled.clear();
  oled.setCursor(10, 0);
  oled.println("Setting Counter to 0");
  delay(1000);
#endif
} // end of setup()
#ifdef USE_MYSENSORS
void presentation()
{
  transmitter.presentation();
}
#endif
/* MySensors presentation
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
*/
/* MySensors receive Function
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
      // ROOM_SENSOR.stopContinuous();
      // CORRIDOR_SENSOR.stopContinuous();
      // VL53LXX_init();

      // calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
      ROOM_SENSOR.calibration();
      CORRIDOR_SENSOR.calibration();
    }

    if (message.sensor == CHILD_ID_PC)
    {
      delay(30);
      Serial.println(message.getInt());
      peopleCount = message.getInt();
      // send(pcMsg.set(peopleCount));
    }
  }
}
*/
int lastState = LOW;
int newState = LOW;

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
#ifdef USE_OLED
    oled.clear();
    oled.setCursor(5, 0);
    oled.setTextSize(2, 1);
    oled.print("Timeout occured!");
#endif
    // reportToController(65535);
    Serial.println("Timeout occured. Restart the System");

    // calibration(ROOM_SENSOR, CORRIDOR_SENSOR);
    ROOM_SENSOR.calibration();
    CORRIDOR_SENSOR.calibration();
  }
  /*
  // Sleep until interrupt comes in on motion sensor. Send never an update
  if (motion.checkMotion() == LOW)
  {
#ifdef USE_OLED
    oled.clear();
#endif
#ifdef USE_ENEGERY_SAVING
    if (lastState == HIGH)
    {
      // readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
#ifdef MY_DEBUG
      Serial.println("Shutting down sensors");
#endif
      ROOM_SENSOR.stopContinuous();
      CORRIDOR_SENSOR.stopContinuous();

      // delay(30);
      // request(CHILD_ID_THR, V_TEXT, 0);
      // delay(30);
      // request(CHILD_ID_PC, V_TEXT, 0);
      lastState = LOW;
    }
#else
    // request(CHILD_ID_THR, V_TEXT, 0);
    // delay(30);
    // request(CHILD_ID_PC, V_TEXT, 0);
    // readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
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
      delay(10);
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
      // readSensorData(ROOM_SENSOR, CORRIDOR_SENSOR);
    }
  }
  */
}

#ifdef USE_MQTT
void callback(char *topic, byte *payload, unsigned int length)
{ // ****************

  DynamicJsonDocument root(MQTT_MAX_PACKET_SIZE);
  String messageReceived = "";

  // Affiche le topic entrant - display incoming Topic
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // decode payload message
  for (int i = 0; i < length; i++)
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