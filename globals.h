#ifndef globals_h
#define globals_h

// PLS Inputs **************************************
bool InverterReady;
bool InterlockSamifi;
bool BtnStart;
bool BtnStop;
bool SafetyRelayStatus;
bool EjectorCylinderPosistion;
bool EmergencyStopMonitor;
uint16_t InverterPowerUsage;

// Sensor rekke som ser om produktet er rett vei
bool EjectorSensorMiddleLeft;
bool EjectorSensorMiddle;
bool EjectorSensorMiddleRight;
bool EjectorSensorEdges;
//***************************************************


byte InverterSetSpeedByte = 0;
int InverterSetSpeedMiliVolt = 0; // Fra 0V til 10V
const float MiliVoltToByteRatio = 0.0255; // Forholdet mellom Byte(255) og MiliVolt (10000)

#endif