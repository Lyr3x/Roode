#ifndef MQTTTransmitter_H
#define MQTTTransmitter_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Transmitter.h>
#include <Configuration.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID "TP-LINK_851F"
#define WLAN_PASS "kdsjifeunrnughdngjdwgbjsbdvfmhtrew21asxdfdswe467h9"

extern const char *topic_Domoticz_IN;  //$$
extern const char *topic_Domoticz_OUT; //$$
class MQTTTransmitter : Transmitter
{

public:
  // ~MQTTTransmitter() {}
  int transmit(String idx, int val = 0, String text = "");
  int receive();
  void reconnect();
  void test();
  void init();
  // WiFi parameters
  byte WiFi_AP = 1;                                                             // The WiFi Access Point we are connected to : 1 = main one or 2 = alternate one
  const char *ssid = "TP-LINK_851F";                                            //$$
  const char *password = "kdsjifeunrnughdngjdwgbjsbdvfmhtrew21asxdfdswe467h9";  //$$
  const char *ssid2 = "FRITZ!Box 6490 Cable";                                   //$$
  const char *password2 = "kdsjifeunrnughdngjdwgbjsbdvfmhtrew21asxdfdswe467h9"; //$$

  // MQTT parameters
  byte willQoS = 0;
  char willMessage[MQTT_MAX_PACKET_SIZE + 1];
  boolean willRetain = false;
  const char *mqtt_server = "192.168.2.90"; //$$ CERTIFICATION=192.168.1.103 $$ PROD=192.168.1.45

  char msgToPublish[MQTT_MAX_PACKET_SIZE + 1];
  const char *MQTT_ID = "Roode";

  struct Devices
  {
    String room_sensor_value = "256";
    String corridor_sensor_value = "257";
    String room_switch = "258";
    String info = "259";
    String peoplecounter = "260";
    String threshold = "261";
  };
  Devices devices;
};
extern WiFiClient espClient;
extern PubSubClient client;
#endif