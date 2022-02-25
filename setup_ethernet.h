#ifndef setup_ethernet_h
#define setup_ethernet_h

#include "SPI.h"
#include "Ethernet.h"

// Network Configuration
byte mac[] = {0XF6, 0XB1, 0X72, 0XDA, 0X6A, 0X2B};
IPAddress ip(192, 168, 1, 21);

EthernetClient ethClient;

void ethInit()
{
    Ethernet.begin(mac, ip);
}

#endif