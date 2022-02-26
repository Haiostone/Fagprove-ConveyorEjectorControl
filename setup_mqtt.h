#ifndef setup_mqtt_h
#define setup_mqtt_h

#include "PubSubClient.h"

void callback(char *topic, byte *payload, unsigned int length) // handle message arrived
{
// Debugging
#ifdef DEBUG
    DEBUG_PRINT("Message arrived [");
    DEBUG_PRINT(topic);
    DEBUG_PRINT("] ");
    for (int i = 0; i < length; i++)
    {
        DEBUG_PRINT((char)payload[i]);
    }
    DEBUG_PRINTLN();
#endif

if (strcmp(topic, "Melbu/ferdigvare/vatpakk/eskelimer/inn/hastTtransp") == 0) {
    payload[length] = '\0'; // NULL terminerer payloadet for å få en string
    InverterSetSpeedMiliVolt = atoi((char *)payload); // konverterer stringen til en integer
}
}

#if CONNECT_BROKER_WITH_DNS
char brokerHostname[] = mqttBrokerAddress;
#else
IPAddress brokerIp(mqttBrokerIp);
#endif

const int brokerPort = mqttBrokerPort;

#if CONNECT_BROKER_WITH_DNS
PubSubClient mqtt(brokerHostname, brokerPort, callback, ethClient);
#else
PubSubClient mqtt(brokerIp, brokerPort, callback, ethClient);
#endif

boolean mqttReconnect()
{
    //Mqtt Login
    const char *clientName = mqttClientName;
#if USE_MQTT_LOGIN
    const char *brokerUser = mqttBrokerUser;
    const char *brokerPass = mqttBrokerPass;
#endif

// Mqtt Will
boolean willRetain = mqttWillRetain;
byte willQoS = mqttWillQoS;
const char *willTopic = mqttWillTopic;
const char *willMessage = mqttWillMessage;
const char *OnMessage = mqttWillOnMessage;

    DEBUG_PRINT("Attempting MQTT connection...");
    #if USE_MQTT_LOGIN
        if (mqtt.connect(clientName, brokerUser, brokerPass, willTopic, willQoS, willRetain, willMessage))
    #else
        if (mqtt.connect(clientName, willTopic, willQoS, willRetain, willMessage))
    #endif
    {
        // Successfull connection to broker
        DEBUG_PRINTLN("connected");
        mqtt.publish(willTopic, OnMessage, willRetain);

        mqtt.subscribe("Melbu/ferdigvare/vatpakk/eskelimer/inn/hastTtransp");
        
    }
    else
    {
        // Failed connection to broker
        DEBUG_PRINTLN(" Failed connection to broker");
    }
    return mqtt.connected();
}

void mqttLoop()
{
    static unsigned long lastReconnectAttempt = 0;

    if (!mqtt.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (!mqttReconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    mqtt.loop();
  }
}

#endif