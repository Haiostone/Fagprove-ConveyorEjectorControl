#ifndef setup_h
#define setup_h

// Comment line below to enable or disable debuging code
#define DEBUG
//*******************************************************

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_WRITE(x, y) Serial.write(x, y)

void debugFunction()
{
  // Set console baud rate
  Serial.begin(9600);
  delay(10);
  // Code for debugging
}
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_WRITE(x, y)
#endif

#endif