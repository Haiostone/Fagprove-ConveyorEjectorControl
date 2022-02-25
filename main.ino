#include "setup.h"
//#include "setup_ethernet.h"
//#include "setup_mqtt.h"

#include "constants.h"
#include "globals.h"
#include "config.h"
#include "functions.h"

int EjectorStatusCode;
int BoxErrorCode;
int BoxErrorState = 0;

void setup() {
#ifdef DEBUG
  debugFunction();
#endif
  // ethInit();
}

void loop() {
  // mqttLoop(); // Connects and keeps mqtt connection alive
  updateInputs();

  BoxErrorCode = BoxError();
  if (BoxErrorCode != 0) BoxErrorState = 10;

  switch (BoxErrorState) {
    case 10:
      EjectorStatusCode = ActivateEjector(300);
      if (EjectorStatusCode == 1) {
        // Utkastet Fulført
        BoxErrorState = BoxErrorCode;
      }
      if (EjectorStatusCode == 2) {
        // Utkastet Feil
        BoxErrorState = BoxErrorCode;
      }
      break;
    case 2:
      // Eske feil: ulimt kant
      BoxErrorState = 0;
      break;

    case 3:
      // Eske feil: skjev
      BoxErrorState = 0;
      break;

    default:
      // Ukjent feil
      BoxErrorState = 0;
      break;
  }


}