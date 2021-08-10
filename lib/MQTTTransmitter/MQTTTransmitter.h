#ifndef MQTTTransmitter_H
#define MQTTTransmitter_H

#include <Config.h>
// #include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Transmitter.h>

// MQTT
#include <PubSubClient.h>

// For storing configurations
#include "FS.h"
#include <ArduinoJson.h>

// WiFiManager Libraries
#include <DNSServer.h>            //Local DNS Server used for redirecting all rs to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


extern const char *topic_Domoticz_IN;
extern const char *topic_Domoticz_OUT;
class MQTTTransmitter : public Transmitter
{

public:
  // ~MQTTTransmitter() {}
  int transmit(String idx, int val = 0, String text = "");
  void callback(char* topic, byte* payload, unsigned int length);
  void reconnect();
  void test();
  void init();
  // WiFi parameters
  byte WiFi_AP = 1; // The WiFi Access Point we are connected to : 1 = main one or 2 = alternate one
  const char *ssid = WLAN_SSID;
  const char *password = WLAN_PASS;
  const char *ssid2 = WLAN_SSID2;
  const char *password2 = WLAN_PASS2;

  // MQTT parameters
  byte willQoS = 0;
  char willMessage[MQTT_MAX_PACKET_SIZE + 1];
  boolean willRetain = false;
  const char *mqtt_server = MQTT_IP;

  char msgToPublish[MQTT_MAX_PACKET_SIZE + 1];
  const char *MQTT_ID = "Roode";
  
  struct Devices
  {
    String room_sensor_value = ROOM_MQTT;
    String corridor_sensor_value = CORRIDOR_MQTT;
    String room_switch = ROOM_SWITCH_MQTT;
    String info = INFO_MQTT;
    String peoplecounter = PEOPLECOUNTER_MQTT;
    String threshold = THRESHOLD_MQTT;
  };
  Devices devices;
};
extern WiFiClient espClient;
extern PubSubClient client;
#endif
