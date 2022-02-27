#include "globals.h"
#include "config.h"

#include "setup.h"
#include "setup_ethernet.h"
#include "setup_mqtt.h"

#include "functions.h"

#include <jled.h>

auto JLedGreen = JLed(PIN_output_PiezoBtnLedGreen);
auto JLedRed = JLed(PIN_output_PiezoBtnLedRed);

int EjectorStatusCode; // Utkaster status kode, om utkastet ble fulført eller ikke
int BoxErrorCode; // Lagrer hvilken type feil på eske det var

int RunState = 1;      // Hvilket steg maskinen er på
int PrevRunState = 0;  // Forrigje steg maskinen var på
bool FirstRunState = true;  // Om det er første syklus maskinen kjører på gjeldende steg

unsigned long SendTimeInverterPowerUsage = 0; // Tiden når omformer strømforbruk sist ble sendt
char StringBufferInverterPowerUsage[8]; // Buffer for å gjøre integer verdien om til string for å sendes via mqtt

unsigned int BoxCount = 0; // Teller antall esker som har passert
unsigned int BoxErrorCount = 0; // Teller totalt anntall esker som har feil og blitt kastet ut
unsigned int BoxErrorCountFlaps = 0; // Teller esker med feilen: dårlig limt kant
unsigned int BoxErrorCountDirection = 0; // Teller esker med feilen: feil rotert

// Buffer for å gjøre eske teller verdiene over om til strings for å sendes via mqtt
char StringBufferBoxCount[8];
char StringBufferBoxErrorCount[8];
char StringBufferBoxErrorCountFlaps[8];
char StringBufferBoxErrorCountDirection[8];

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

  TimeNow = millis(); // Lagrer tiden i en variabel slik at millis bare trenger å kjøre en gang per loop

  if (TimeNow - SendTimeInverterPowerUsage >= SendPeriodInverterPowerUsage) {
    SendTimeInverterPowerUsage = TimeNow;
    itoa(InverterPowerUsage, StringBufferInverterPowerUsage, 10); // Konverterer integer verdi til string som mqtt kan sende
    mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/lastTransp", StringBufferInverterPowerUsage);
  }

  if (!InverterReady) RunState = 10; // Hvis omformer ikker er klar, for eksempel vis den slår ut på overstrøm, så stopper maskinen
  if (!SafetyRelayStatus && EmergencyStopMonitor) RunState = 0; // Sikkerhetsrele åpen, og nødstopp bryter fortsatt aktiv
  else if (!SafetyRelayStatus && !EmergencyStopMonitor) RunState = 1; // Sikkerhetsrele åpen, og nødstopp bryter har blitt reset

  if (PrevRunState != RunState) FirstRunState = true;
  PrevRunState = RunState;

  switch (RunState) {
    case 0:
      // Nødstopp - Kan ikke reset **********************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Off();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Off();

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "2");
      }

      break;

    case 1:
      // Nødstopp - Kan reset ***************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Blink(500, 500).Forever();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Off();

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "5");
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

    case 10:
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Off();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().Off();

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "3");
      }

      if (InverterReady) RunState = 1;
      break;

    case 2:
      // Maskin stoppet ******************************************************
      if (FirstRunState) {
        digitalWrite(PIN_output_InverterStart, false);
        digitalWrite(PIN_output_InverterSpeedSignal, false);
        digitalWrite(PIN_output_EjectorCylinderValve, false);

        JLedRed = JLed(PIN_output_PiezoBtnLedRed).LowActive().Off();
        JLedGreen = JLed(PIN_output_PiezoBtnLedGreen).LowActive().On();

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "0");
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

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "1");
      }

      analogWrite(PIN_output_InverterSpeedSignal, InverterSetSpeedByte); // Setter hastigheten på omformer til verdi mottat fra HMI

      // Kjører utkaster funksjonene og behandler status kodene
      BoxErrorCode = BoxErrorDetect();
      if (BoxErrorCode != 0 && BoxErrorCode != 3) {
        DEBUG_PRINTLN(BoxErrorCode);
        EjectorStatusCode = 0;
        while (!EjectorStatusCode) {
          updateInputs();
          EjectorStatusCode = ActivateEjector(EjectorActivateDelay);
          if (EjectorStatusCode == 1) {
            // Utkastet Fulført
            DEBUG_PRINTLN("Utkastet fulført");
            // Inkrementerer verdi med antall esker med feil, og sender antallet
            itoa(++BoxErrorCount, StringBufferBoxErrorCount, 10); // Konverterer integer verdi til string som mqtt kan sende
            mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/feil", StringBufferBoxErrorCount);
          }
          if (EjectorStatusCode == 2) {
            // Utkastet Feil
            DEBUG_PRINTLN("Utkastet feil");
            mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/feilUtkaster", "Feil på utkaster mekanisme");
          }
        }
        if (BoxErrorCode == 1) {
          // Eske feil: ulimt kant
          DEBUG_PRINTLN("ulimt kant");
          // Inkrementerer verdi med antall esker med feil, og sender antallet
          itoa(++BoxErrorCountFlaps, StringBufferBoxErrorCountFlaps, 10); // Konverterer integer verdi til string som mqtt kan sende
          mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/feilUlimt", StringBufferBoxErrorCountFlaps);
        }

        else if (BoxErrorCode == 2) {
          // Eske feil: skjev
          DEBUG_PRINTLN("skjev eske");
          // Inkrementerer verdi med antall esker med feil, og sender antallet
          itoa(++BoxErrorCountDirection, StringBufferBoxErrorCountDirection, 10); // Konverterer integer verdi til string som mqtt kan sende
          mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/feilRotert", StringBufferBoxErrorCountDirection);
        }
      }
      else if (BoxErrorCode == 3) {
        // Eske OK
        DEBUG_PRINTLN("Eske OK");
        // Inkrementerer verdi med antall esker, og sender antallet
        itoa(++BoxCount, StringBufferBoxCount, 10); // Konverterer integer verdi til string som mqtt kan sende
        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/telleverk", StringBufferBoxCount);
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

        mqtt.publish("Melbu/ferdigvare/vatpakk/eskelimer/ut/status", "4");
      }

      analogWrite(PIN_output_InverterSpeedSignal, InverterSetSpeedByte);

      if (InterlockSamifi) RunState = 3;
      if (BtnStop) RunState = 2;
      break;
  }
  FirstRunState = false;
}
