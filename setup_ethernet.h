#ifndef setup_ethernet_h
#define setup_ethernet_h

#include "SPI.h"
#include "Ethernet2.h"

EthernetClient ethClient;

byte ethInit()
{
  byte mac[] = ethernetMac;
#if !ETHERNET_USE_DHCP
  IPAddress ip(ethernetIp);
#endif

// DHCP
#if ETHERNET_USE_DHCP
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0)
  {
    DEBUG_PRINTLN("Failed to configure Ethernet using DHCP");
    return (1);
  }

// print your local IP address:
#ifdef DEBUG
  DEBUG_PRINT("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++)
  {
    // print the value of each byte of the IP address:
    DEBUG_PRINT(Ethernet.localIP()[thisByte]);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN();
#endif
  return (0);
#else
  Ethernet.begin(mac, ip);
#endif
}

#endif