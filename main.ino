#include "globals.h"
#include "config.h"

#include "setup.h"
#include "setup_ethernet.h"
#include "setup_mqtt.h"

#include "constants.h"
#include "functions.h"

#include <jled.h>

auto JLedGreen = JLed(PIN_output_PiezoBtnLedGreen);
auto JLedRed = JLed(PIN_output_PiezoBtnLedRed);

int EjectorStatusCode; // Utkaster status kode, om utkastet ble fulført eller ikke
int BoxErrorCode; // Lagrer hvilken type feil på eske det var
int EjectorState = 0; // Hvilket steg utkasteren er på

int RunState = 1;      // Hvilket steg maskinen er på
int PrevRunState = 0;  // Forrigje steg maskinen var på
bool FirstRunState = true;  // Om det er første syklus maskinen kjører på gjeldende steg

void setup() {
#ifdef DEBUG
  debugFunction();
#endif
  ethInit();
}


void loop() {

  #if ETHERNET_USE_DHCP
    Ethernet.maintain(); // Hvis DHCP er brukt for ip addresse, oppdatererer denne ip
  #endif

  mqttLoop(); // Kobler til mqtt broker // Holder mqtt i livet

  updateInputs(); // Oppdaterer variablene til PLS inputet
  JLedGreen.Update(); // Oppdaterer Led state
  JLedRed.Update(); // Oppdaterer Led state


  InverterSetSpeedByte = InverterSetSpeedMiliVolt * MiliVoltToByteRatio;
  //DEBUG_PRINTLN(InverterSetSpeedByte);

  if (!SafetyRelayStatus && EmergencyStopMonitor) RunState = 0; // Sikkerhetsrele åpen, og nødstopp bryter fortsatt aktiv
  else if (!SafetyRelayStatus && !EmergencyStopMonitor) RunState = 1; // Sikkerhetsrele åpen, og nødstopp bryter har blitt reset

  if (PrevRunState != RunState) FirstRunState = true;
  PrevRunState = RunState;

  switch (RunState) {
    case 0:
      // Nødstopp aktivert - Kan ikke reset **********************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Off();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Off();
      }

      break;

    case 1:
      // Nødstopp aktivert - Kan reset ***************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Blink(500, 500).Forever();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Off();
      }

      if (BtnStop) {
        // Lukker rele som reseter sikkerhetsrele
        if (SafetyRelayStatus) RunState = 2;
        else {
          digitalWrite(PIN_output_SafetyRelayReset, HIGH);
          delay(100);
          digitalWrite(PIN_output_SafetyRelayReset, LOW);
          delay(200);
          updateInputs();
          if (SafetyRelayStatus) RunState = 2;
        }
      }
      break;

    case 2:
      // Maskin stoppet ******************************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Off();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().On();
      }

      if (BtnStart && !BtnStop) {
        RunState = 3;
      }
      break;

    case 3:
      // Maskin kjører *******************************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, true);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().On();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().On();
      }

      analogWrite(PIN_output_InverterSpeedSignal, InverterSetSpeedByte); // Setter hastigheten på omformer til verdi mottat fra HMI
      
      // Kjører utkaster funksjonene og behandler status kodene
      BoxErrorCode = BoxErrorDetect();
      if (BoxErrorCode != 0) EjectorState = 10;

      switch (EjectorState) {
        case 10:
          EjectorStatusCode = ActivateEjector(EjectorActivateDelay);
          if (EjectorStatusCode == 1) {
            // Utkastet Fulført
            DEBUG_PRINTLN("Utkastet fulført");
            EjectorState = BoxErrorCode;
          }
          if (EjectorStatusCode == 2) {
            // Utkastet Feil
            DEBUG_PRINTLN("Utkastet feil");
            EjectorState = BoxErrorCode;
          }
          break;
        case 2:
          // Eske feil: ulimt kant
          DEBUG_PRINTLN("ulimt kant");
          EjectorState = 0;
          break;

        case 3:
          // Eske feil: skjev
          DEBUG_PRINTLN("skjev eske");
          EjectorState = 0;
          break;

        default:
          // Ukjent feil
          EjectorState = 0;
          break;
      }

      if (!InterlockSamifi) RunState = 4;
      if (BtnStop) RunState = 2;
      break;

    case 4:
      // Maskin kjører - Venter på SAMIFI ************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().On();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Blink(500, 500).Forever();
      }

      if (InterlockSamifi) RunState = 3;
      if (BtnStop) RunState = 2;
      break;
  }
  
  if(FirstRunState) {
    mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", )
  }
  
  FirstRunState = false;
}
