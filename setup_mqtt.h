#ifndef setup_mqtt_h
#define setup_mqtt_h

#include "PubSubClient.h"

// MQTT details *************************************************************

IPAddress brokerIp(34, 215, 212, 114);
const int brokerPort = 1883;

const char *clientName = "arduinoTestClient";

const char *brokerUser = "arduinoClient";
const char *brokerPass = "arduinoClient";
//**************************************************************************

void callback(char *topic, byte *payload, unsigned int length)
{
    // handle message arrived
    DEBUG_PRINT("Message arrived [");
    DEBUG_PRINT(topic);
    DEBUG_PRINT("] ");
    for (int i = 0; i < length; i++)
    {
        DEBUG_PRINT((char)payload[i]);
    }
    DEBUG_PRINTLN();
}

PubSubClient mqtt(brokerIp, brokerPort, callback, gprsClient);

void reconnect()
{
    // Loop until we're reconnected
    while (!mqtt.connected())
    {
        DEBUG_PRINT("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt.connect(clientName))//, brokerUser, brokerPass)) // If no login is needed, remove the "brokerUser" and "brokerPass" variables
        {
            // Successfull connection to broker
            DEBUG_PRINTLN("connected");
            mqtt.publish("outTopic", "hello world");
            mqtt.subscribe("*");
        }
        else
        {
            // Failed connection to broker
            DEBUG_PRINT("failed, rc=");
            DEBUG_PRINT(mqtt.state());
            DEBUG_PRINTLN(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqttLoop()
{
    reconnect(); // Reconnects if arduino loses connection with broker
    mqtt.loop(); // Keeps connection alive
}

#endif