#ifndef config_h
#define config_h

#include "credentials.h"

// Enables debugging code ***************************************************
#define DEBUG

// Network Configuration ****************************************************
#define ETHERNET_USE_DHCP true
#define ethernetMac {0xB6, 0x14, 0x44, 0x7B, 0x1C, 0xCB}
//#define ethernetIp 192, 168, 1, 50

// MQTT Parameters *************************************************************
#define USE_MQTT_LOGIN true
#define USE_MQTT_ETHERNET true
#define CONNECT_BROKER_WITH_DNS true

#define MQTT_MAX_PACKET_SIZE 1024

#if CONNECT_BROKER_WITH_DNS
#define mqttBrokerAddress "broker.melbu.leroy.no"
#else
#define mqttBrokerIp 10, 127, 0, 25
#endif
#define mqttBrokerPort 1883
#define mqttClientName "VapUtkaster"
#define mqttBrokerUser CredentialMqttBrokerUser
#define mqttBrokerPass CredentialMqttBrokerPass

#define mqttTimeTopic "Melbu/IIoT/UTCsync"

// MQTT Last will and testament
#define mqttWillRetain true
#define mqttWillQoS 0
#define mqttWillTopic "Melbu/ferdigvare/vatpakk/eskelimer/ut/PLS_Status"
#define mqttWillMessage "Offline"
#define mqttWillOnMessage "Online"

// Mqtt payload parameters
#define payloadSendPeriod 60000
#define payloadRbeTimeout 500
#define payloadTopic "Melbu/ferdigvare/vatpakk/eskelimer/ut"


// Function Parameters *******************************************************************************

// Eske feil detektering 
const unsigned int ScanWaitPeriod = 100; // Forsinkelse i ms, fra esken først blir sett av en sensor til en lesning fra alle sensorene tas
const unsigned int ScanNewBoxPeriod = 1000; // Forsinkelse i ms, mellom eske detektering

// Utkaster
const unsigned int EjectorActivateDelay = 425; // Forsinkelse i ms, fra funksjon blir kalt til utkastning begynner 
const unsigned int EjectorOutInDelay = 0; // Forsinkelse i ms, mellom utkaster sylinder går ut, til den skal inn igjen
const unsigned int EjectorOutInTimeout = 500; // Forsinkelse i ms, for timeout på utkaster sylinder vis sylinder sensor aldri blir aktivert


#endif