#ifndef globals_h
#define globals_h

// PLS Inputs *************************************************
const int PIN_input_InverterReady = I0_0;  // Signal fra omformer når den er klart til å starte
const int PIN_input_InterlockSAMIFI = I0_1; // Forrigling med SAMIFI platefryser
const int PIN_input_PiezoBtnSwitchStart = I0_2; // Piezo bryter på skap som starter maskin
const int PIN_input_PiezoBtnSwitchStop = I0_3; // Piezo bryter på skap som stopper maskin 
const int PIN_input_EmergencyStopMonitor = I0_4; // Normally open kontakt blokk på nødstopp for monitoring
const int PIN_input_SafetyRelayStatus = I0_5; // Rele signal fra sikkerhetsreleen for å vite når maskinen har stoppet på grunn av nødstopp

// Sensor rekke som ser om produktet er rett vei
const int PIN_input_EjectorProductSensor_MiddleLeft = I0_7;
const int PIN_input_EjectorProductSensor_Middle = I0_8;
const int PIN_input_EjectorProductSensor_MiddleRight = I0_9;
const int PIN_input_EjectorProductSensor_Edges = I0_10;

const int PIN_input_EjectorCylinderPositionSensor = I0_11; // Utkaster sylinder posisjonssensor, ser når sylinder er helt ute
const int PIN_input_InverterPowerUsageSignal = I0_12; // Analog signal fra omformer om strømforbruket til motoren
//***************************************************************



// PLS Outputs ***********************************************
const int PIN_output_SafetyRelayReset = Q0_0; // En egen rele som lukker reset kontaktene på sikkerhetsreleens reset terminaler
const int PIN_output_InverterStart = Q0_1; // Starter omformeren
const int PIN_output_EjectorCylinderValve = Q0_2; // Magnetventil til utkaster sylinder
const int PIN_output_PiezoBtnLedGreen = Q0_3; // Piezo bryter integrert grønt led lys
const int PIN_output_PiezoBtnLedRed = Q0_4; // Piezo bryter integrert rødt led lys
const int PIN_output_InverterSpeedSignal = A0_5; // Analog 0 til 10V signal for endring av hastighet på bånd
//*************************************************************


// Variabler som holder verdien til inngangen
// PLS Inputs **************************************
bool InverterReady;
bool InterlockSamifi;
bool BtnStart;
bool BtnStop;
bool SafetyRelayStatus;
bool EjectorCylinderPosistion;
bool EmergencyStopMonitor;
unsigned int InverterPowerUsage;

// Sensor rekke som ser om produktet er rett vei
bool EjectorSensorMiddleLeft;
bool EjectorSensorMiddle;
bool EjectorSensorMiddleRight;
bool EjectorSensorEdges;
//***************************************************

// Minne for hastighetsregulering på omformer
byte InverterSetSpeedByte = 0; // Byte verdi som blir skrevet til analog utgang
int InverterSetSpeedMiliVolt = 0; // Milivolt verdi mellom 0mV til 10000mV, kommer fra HMI via MQTT
const float MiliVoltToByteRatio = 0.0255; // Forholdet mellom Byte(255) og MiliVolt (10000)

unsigned long TimeNow = 0; // Minne for å holde på verdi fra Millis funksjon for å unngå å bruke funksjon flere ganger iløpet av en loop


#endif