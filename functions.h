#ifndef functions_h
#define functions_h

#include "globals.h"

void updateInputs() {
  // Oppdaterer variabler for Innganger

  InverterReady = digitalRead(PIN_input_InverterReady);
  InterlockSamifi = digitalRead(PIN_input_InterlockSAMIFI);
  BtnStart = digitalRead(PIN_input_PiezoBtnSwitchStart);
  BtnStop = digitalRead(PIN_input_PiezoBtnSwitchStop);
  SafetyRelayStatus = digitalRead(PIN_input_SafetyRelayStatus);
  EjectorCylinderPosistion = digitalRead(PIN_input_EjectorCylinderPositionSensor);
  EmergencyStopMonitor = digitalRead(PIN_input_EmergencyStopMonitor);
  InverterPowerUsage = analogRead(PIN_input_InverterPowerUsageSignal);

  // Sensor rekke som ser om produktet er rett vei
  EjectorSensorMiddleLeft = digitalRead(PIN_input_EjectorProductSensor_MiddleLeft);
  EjectorSensorMiddle = digitalRead(PIN_input_EjectorProductSensor_Middle);
  EjectorSensorMiddleRight = digitalRead(PIN_input_EjectorProductSensor_MiddleRight);
  EjectorSensorEdges = digitalRead(PIN_input_EjectorProductSensor_Edges);
}

int BoxErrorDetect() {
  static unsigned long TimeSinceSensorActivation = 0;
  static unsigned long TimeSinceBoxPassed = 0;

  static int state = 0;

  int ReturnCode = 0;

  switch (state) {
    case 0: 
    // Lagrer tiden når en eske først blir sett av en sensor
      if (EjectorSensorEdges || EjectorSensorMiddle || EjectorSensorMiddleLeft || EjectorSensorMiddleRight) {
        TimeSinceSensorActivation = millis();
        state = 1;
      }
      break;

    case 1: 
    // Forsinkelse på en bestemt tid fra en eske først har blit sett
      if (millis() - TimeSinceSensorActivation >= ScanWaitPeriod) {
        state = 2;
      }
      break;

    case 2: 
    // Hvis en sensor på ytterste kant blir aktivert, er det en feil limt eske
      if (EjectorSensorEdges) {
        state = 4;
        ReturnCode = 1;
      } else
        state = 3;
      break;

    case 3: 
    // Hvis ikke alle tre miderste sensorer ser esken, er esken skjev
      if (EjectorSensorMiddle && EjectorSensorMiddleLeft && EjectorSensorMiddleRight) {
        state = 4;
        ReturnCode = 3;
      } else {
        state = 4;
        ReturnCode = 2;
      }
      break;

      case 4: 
      // Lagrer tiden når esken passerte
        TimeSinceBoxPassed = millis();
        state = 5;
      break;

      case 5: 
      // Forsinkelse på en sett verdi etter esken har passert før den sjekker etter ny eske.
        if(millis() - TimeSinceBoxPassed + ScanWaitPeriod >= ScanNewBoxPeriod) {
            state = 0;
        }
      break;
  }
  return ReturnCode;
/*
Return Koder:
0 = Alt OK
1 = Eske feil: dårlig limt
2 = Eske feil: skjev
3 = Eske Passert uten feil
*/
}


int ActivateEjector(int EjectorDelay) {

static unsigned long TimeSinceRequest = 0; // Tiden når funksjon for utkaster først blir kalt
static unsigned long TimeSinceEjectorExtended = 0; // Tiden når magnetventil for utkaster sylinder ble aktivert

static int state = 0;

int ReturnCode = 0;

switch (state)
{
case 0: 
// Lagrer tiden for når funksjonen først blir kalt
  TimeSinceRequest = millis();
  state = 1;
  break;

case 1: 
// Forsinkelse på en bestent tid for når funksjonen først blir kalt, til sylinderet skal aktiveres (Endres basert på avstand mellom sensorer og utkaster)
if(millis() - TimeSinceRequest >= EjectorDelay){
  digitalWrite(PIN_output_EjectorCylinderValve, true);
  state = 2;
}
break;
case 2: 
// Sjekker om utkaster sylinder er helt ute, vis den er det så lagrer den tiden når den kom ut. Hvis ikke så venter den på Timeout forsinkelse før den sender feil
if(EjectorCylinderPosistion) {
TimeSinceEjectorExtended = millis();
state = 3;
}
else if(millis() - TimeSinceRequest >= EjectorOutInTimeout){
  state = 4;
}
break;
case 3: 
// Når bestemt tid for sylinder aktivert er nådd, så sendes sylinder inn igjen. 
if(millis() - TimeSinceEjectorExtended >= EjectorOutInDelay) {
  digitalWrite(PIN_output_EjectorCylinderValve, false);
  ReturnCode = 1;
  state = 0;
}
break;
case 4: 
// Vis feil oppstår, send feilkode
 digitalWrite(PIN_output_EjectorCylinderValve, false);
  ReturnCode = 2;
  state = 0;
break;
}

return ReturnCode;
/*
Return Koder:
0 = Utkastning begynt
1 = Utkastning fulført
2 = Utkastning feil
*/
}

#endif