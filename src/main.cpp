#include <Arduino.h>
#include "digital_io.h"
#include "screen_control.h"
#include "termo_k.h"
#include "max6675.h"

// int thermoDO = 19;
// int thermoCS = 23;
// int thermoCLK = 5;

// MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  delay(500);
  Serial.begin(115200);
  _10klab::digitalIO::setupIO();
  _10klab::screen::SetUpScreen();
  _10klab::termo_k::SetupPID();
  _10klab::termo_k::SetPIDMode(true);
  // _10klab::digitalIO::selectPIDParameters();
}




void loop() {

_10klab::digitalIO::selectPIDParameters();
_10klab::termo_k::ProfilePIDProcess();

}