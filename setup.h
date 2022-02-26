#ifndef setup_h
#define setup_h

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_WRITE(x, y) Serial.write(x, y)

void debugFunction()
{
  // Set console baud rate
  Serial.begin(9600);
  // Code for debugging
}
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_WRITE(x, y)
#endif

#endif