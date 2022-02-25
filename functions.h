#ifndef functions_h
#define functions_h

#include "globals.h"

void updateInputs() {
  InverterReady = digitalRead(PIN_input_InverterReady);
  InterlockSamifi = digitalRead(PIN_input_InterlockSAMIFI);
  BtnStart = digitalRead(PIN_input_PiezoBtnSwitchStart);
  BtnStop = digitalRead(PIN_input_PiezoBtnSwitchStop);
  SafetyRelayStatus = digitalRead(PIN_input_SafetyRelayStatus);
  EjectorCylinderPosistion = digitalRead(PIN_input_EjectorCylinderPositionSensor);
  InverterPowerUsage = analogRead(PIN_input_InverterPowerUsageSignal);

  // Sensor rekke som ser om produktet er rett vei
  EjectorSensorMiddleLeft = digitalRead(PIN_input_EjectorProductSensor_MiddleLeft);
  EjectorSensorMiddle = digitalRead(PIN_input_EjectorProductSensor_Middle);
  EjectorSensorMiddleRight = digitalRead(PIN_input_EjectorProductSensor_MiddleRight);
  EjectorSensorEdges = digitalRead(PIN_input_EjectorProductSensor_Edges);
}

int BoxError() {

  static unsigned long TimeSinceSensorActivation = 0;
  static unsigned int TimeSinceBoxPassed = 0;

  unsigned int ScanWaitPeriod = 200;
  unsigned int ScanNewBoxPeriod = 1000;

  static int state = 0;
  int ErrorCode = 0;

  switch (state) {
    case 0:
      if (EjectorSensorEdges || EjectorSensorMiddle || EjectorSensorMiddleLeft || EjectorSensorMiddleRight) {
        TimeSinceSensorActivation = millis();
        state = 1;
      }
      break;

    case 1:
      if (millis() - TimeSinceSensorActivation >= ScanWaitPeriod) {
        state = 2;
      }
      break;

    case 2:
      if (EjectorSensorEdges) {
        state = 4;
        ErrorCode = 1;
      } else
        state = 3;
      break;

    case 3:
      if (EjectorSensorMiddle && EjectorSensorMiddleLeft && EjectorSensorMiddleRight) {
        state = 4;
        ErrorCode = 0;
      } else {
        state = 4;
        ErrorCode = 2;
      }
      break;

      case 4:
        TimeSinceBoxPassed = millis();
        state = 5;
      break;

      case 5:
        if(millis() - TimeSinceBoxPassed >= ScanNewBoxPeriod) {
            state = 0;
        }
      break;
  }
  return ErrorCode;
}


int ActivateEjector(int EjectorDelay) {
static unsigned long TimeSinceRequest = 0;
static unsigned long TimeSinceEjectorExtended = 0;

static int ejectorState = 0;

int ReturnCode = 0;

int EjectorExtendedDelay = 300;
int EjectorExtendTimeout = 1000;

switch (ejectorState)
{
case 0:
  TimeSinceRequest = millis();
  ejectorState = 1;
  break;

case 1:
if(millis() - TimeSinceRequest >= EjectorDelay){
  digitalWrite(PIN_output_EjectorCylinderValve, true);
  ejectorState = 2;
}
break;
case 2:
if(EjectorCylinderPosistion) {
TimeSinceEjectorExtended = millis();
ejectorState = 3;
}
else if(millis() - TimeSinceRequest >= EjectorExtendTimeout){
  ejectorState = 4;
}
break;
case 3:
if(millis() - TimeSinceEjectorExtended >= EjectorExtendedDelay) {
  digitalWrite(PIN_output_EjectorCylinderValve, false);
  ReturnCode = 1;
  ejectorState = 0;
}
break;
case 4:
 digitalWrite(PIN_output_EjectorCylinderValve, false);
  ReturnCode = 2;
  ejectorState = 0;
break;
}

return ReturnCode;
}

#endif