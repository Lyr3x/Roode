#include <MQTTTransmitter.h>
WiFiClient espClient;
PubSubClient client(espClient);
void MQTTTransmitter::init()
{
    // Serial.begin(115200);
    // // Connect to WiFi access point.
    // Serial.println();
    // Serial.println();
    // Serial.print("Connecting to ");
    // Serial.println(ssid);

    // // connect to WiFi Access Point
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, password);
    // while (WiFi.waitForConnectResult() != WL_CONNECTED)
    // {
    //     Serial.println("Connection to the main WiFi Failed!");
    //     delay(2000);
    //     if (WiFi_AP == 1)
    //     {
    //         WiFi_AP = 2;
    //         Serial.println("Trying to connect to the alternate WiFi...");
    //         WiFi.begin(ssid2, password2);
    //     }
    //     else
    //     {
    //         WiFi_AP = 1;
    //         Serial.println("Trying to connect to the main WiFi...");
    //         WiFi.begin(ssid, password);
    //     }
    // }
    // //MQTT
    // client.setServer(mqtt_server, 1883);
    // client.setCallback(callback);

    // // say we are now ready and give configuration items
    // Serial.println("Ready");
    // Serial.print("Connected to ");
    // if (WiFi_AP == 1)
    //     Serial.println(ssid);
    // else
    //     Serial.println(ssid2);
    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());
}
int MQTTTransmitter::receive()
{
    //needs to be implemented
}
// int MQTTTransmitter::send()
// {
//     Serial.println("Call send(String idx, int val, String text");
//     return -1;
// }
int MQTTTransmitter::transmit(String idx, int val, String text)
{
    String string;
    if (idx == ROOM_MQTT || idx == CORRIDOR_MQTT)
    {
        string = "{\"command\" : \"udevice\", \"idx\" : " + idx + ", \"nvalue\": " + 0 + ", \"svalue\": \"" + val + "\"}";
    }
    else if (idx == ROOM_SWITCH)
    {
        if (val == 1)
        {
            string = "{\"command\" : \"switchlight\", \"idx\" : " + idx + ", \"switchcmd\": \"" + "On" + "\"}";
        }
        else if (val == 0)
        {
            string = "{\"command\" : \"switchlight\", \"idx\" : " + idx + ", \"switchcmd\": \"" + "Off" + "\"}";
        }
    }
    else if (idx == INFO)
    {
        string = "{\"command\" : \"udevice\", \"idx\" : " + idx + ", \"nvalue\": " + 0 + ", \"svalue\": \"" + text + "\"}";
    }
    else if (idx == PEOPLECOUNTER)
    {
        string = "{\"command\" : \"udevice\", \"idx\" : " + idx + ", \"svalue\": \"" + val + "\"}";
    }
    else if (idx == THRESHOLD)
    {
        string = "{\"command\" : \"udevice\", \"idx\" : " + idx + ", \"nvalue\": " + 0 + ", \"svalue\": \"" + text + "\"}";
    }

    string.toCharArray(msgToPublish, MQTT_MAX_PACKET_SIZE);
    //Serial.print(msgToPublish);
    Serial.print(" Published to domoticz/in. Status=");
    if (client.publish(topic_Domoticz_IN, msgToPublish))
    {
        Serial.println("OK");
        return 1;
    }
    else
    {
        Serial.println("FAILED");
        return -1;
    }
    return 0;
}

void MQTTTransmitter::reconnect()
{ // ****************
    // Loop until we're reconnected
    while (!client.connected())
    {
        yield();
        Serial.print("Attempting MQTT connection...");
        String string;

        // Attempt to connect
        string = "{\"command\" : \"addlogmessage\", \"message\" : \"Roode went Offline - IP : " + WiFi.localIP().toString() + "\"}";
        string.toCharArray(willMessage, MQTT_MAX_PACKET_SIZE);
        // if ( client.connect(buf) ) {
        if (client.connect(MQTT_ID, topic_Domoticz_IN, willQoS, willRetain, willMessage))
        {
            Serial.println("connected");

            // suscribe to MQTT topics
            Serial.print("Subscribe to domoticz/out topic. Status=");
            if (client.subscribe(topic_Domoticz_OUT, 0))
                Serial.println("OK");
            else
                Serial.println("FAILED");

            delay(100);
            string = "{\"command\" : \"addlogmessage\", \"message\" : \"RooDe v1.0-alpha MQTT - IP : " + WiFi.localIP().toString() + "\"}";
            string.toCharArray(msgToPublish, MQTT_MAX_PACKET_SIZE);
            Serial.print(msgToPublish);
            Serial.print("Published to domoticz/in. Status=");
            if (client.publish(topic_Domoticz_IN, msgToPublish))
                Serial.println("OK");
            else
                Serial.println("FAILED");
        }
        else
        {
            Serial.print("MQTT connection failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in a second");
            // Wait 1 seconds before retrying
            delay(1000);
        } // if (client.connect
    }     // while (!client.connected()) {
} // void reconnect() { ****************
