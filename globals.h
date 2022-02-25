#ifndef globals_h
#define globals_h

// PLS Inputs **************************************
bool InverterReady;
bool InterlockSamifi;
bool BtnStart;
bool BtnStop;
bool SafetyRelayStatus;
bool EjectorCylinderPosistion;
uint16_t InverterPowerUsage;

// Sensor rekke som ser om produktet er rett vei
bool EjectorSensorMiddleLeft;
bool EjectorSensorMiddle;
bool EjectorSensorMiddleRight;
bool EjectorSensorEdges;
//***************************************************

#endif