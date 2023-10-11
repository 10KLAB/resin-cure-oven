#include "PID_v1.h"
#include "global.h"
#include "termo-k.h"
#include <Arduino.h>

namespace _10klab {
namespace PIDControl {

static double PID_input = 0;
static double PID_output = 0;
const double Kp = 25;
const double Ki = 15;
const double Kd = 1;
const int sampling_time = 300;

const int PID_PIN = 16;
const int pwm_freq = 5000;
const int pwm_channel = 0;
const int pwm_resolution = 8;

PID myPID(&PID_input, &PID_output, &_10klab::global::set_point, Kp, Ki, Kd,
          DIRECT);

int SeconToMillis(int seconds) {
  const int factor = 1000;
  return seconds * factor;
}

void SetPID() {
  ledcSetup(pwm_channel, pwm_freq, pwm_resolution);
  ledcAttachPin(PID_PIN, pwm_channel);
}
void SetPIDMode(bool mode) {
  if (mode) {
    myPID.SetMode(AUTOMATIC);
  } else {
    myPID.SetMode(MANUAL);
  }
}

void PIDControlAction() {
  static unsigned long current_time = 0;
  if (millis() < current_time + sampling_time) {
    return;
  }
  myPID.Compute();
  ledcWrite(pwm_channel, PID_output);
  current_time = millis();
}

void PreheatPlateStepOne() {
  const int preheat_set_point = 150;
  PID_input = _10klab::termo_k::ReadCelsius();
  _10klab::global::set_point = preheat_set_point;

  while (PID_input < preheat_set_point) {
    PID_input = _10klab::termo_k::ReadCelsius();
    _10klab::global::current_temperature = PID_input;
    _10klab::global::ShowTemperature();
    PIDControlAction();
  }
}

void PreheatPlateStepTwo() {
  const int preheat_time = SeconToMillis(150);
  unsigned long start_time = 0;
  static unsigned long current_time = millis();

  current_time = millis();
  start_time = millis();
  while (current_time < start_time + preheat_time) {
    PID_input = _10klab::termo_k::ReadCelsius();
    _10klab::global::current_temperature = PID_input;
    _10klab::global::ShowTemperature();
    PIDControlAction();
    current_time = millis();
  }
}

void ReflowStepOne() {
  const int preheat_set_point = 250;
  PID_input = _10klab::termo_k::ReadCelsius();
  _10klab::global::set_point = preheat_set_point;

  while (PID_input < preheat_set_point) {
    PID_input = _10klab::termo_k::ReadCelsius();
    _10klab::global::current_temperature = PID_input;
    _10klab::global::ShowTemperature();
    PIDControlAction();
  }
}

void ReflowStepTwo() {
  const int preheat_time = SeconToMillis(30);
  unsigned long start_time = 0;
  static unsigned long current_time = millis();

  current_time = millis();
  start_time = millis();
  while (current_time < start_time + preheat_time) {
    PID_input = _10klab::termo_k::ReadCelsius();
    _10klab::global::current_temperature = PID_input;
    _10klab::global::ShowTemperature();
    PIDControlAction();
    current_time = millis();
  }
}

void CoolingStep() {
  _10klab::global::set_point = 30;
  PID_input = _10klab::termo_k::ReadCelsius();

  while (PID_input > _10klab::global::set_point) {
    PID_input = _10klab::termo_k::ReadCelsius();
    _10klab::global::current_temperature = PID_input;
    _10klab::global::ShowTemperature();
    PIDControlAction();
  }
}

void ProfilePIDProcess() {
  PreheatPlateStepOne();
  PreheatPlateStepTwo();
  ReflowStepOne();
  ReflowStepTwo();
  CoolingStep();
  Serial.println("terminé");
  delay(10000);
}

} // namespace PIDControl
} // namespace _10klab