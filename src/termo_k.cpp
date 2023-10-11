#include "termo_k.h"
#include "max6675.h"
#include "SimpleKalmanFilter.h"
#include "PID_v1.h"
#include "screen_control.h"
#include "digital_io.h"


namespace _10klab {
namespace termo_k {
const int thermo_DO = 19;
const int thermo_CS = 23;
const int thermo_CLK = 5;

MAX6675 thermocouple(thermo_CLK, thermo_CS, thermo_DO);

SimpleKalmanFilter simpleKalmanFilter(2, 2, 0.01);

static double PID_input = 0;
static double PID_output = 0;
double Kp = 60;
double Ki = 4;
double Kd = 10;
const int sampling_time = 150;

double setpoint = 60;

const int PID_PIN = 2;
const int pwm_freq = 5000;
const int pwm_channel = 0;
const int pwm_resolution = 8;

int preheat_time = 0;
int heat_time = 0;
int preheat_setpoint = 0;
int heat_setpoint = 0;


PID myPID(&PID_input, &PID_output, &setpoint, Kp, Ki, Kd, DIRECT);

float ReadCelsius(){
    const float compensation = 3;
    int update_time = 300;
    static unsigned long current_time = 0;
    static float temperature = 0;
    static float estimated_temp = 0;


    if(millis() >= current_time + update_time){
        temperature = thermocouple.readCelsius() - compensation;
        estimated_temp = simpleKalmanFilter.updateEstimate(temperature);
        current_time = millis();
    }

    return estimated_temp;
}

int SeconToMillis(int seconds) {
  const int factor = 1000;
  return seconds * factor;
}

void SetupPID() {
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
  static bool agresive_temp_down = false;
  if (millis() < current_time + sampling_time) {
    return;
  }
  myPID.Compute();
  if(ReadCelsius() > setpoint){
    if(agresive_temp_down){
      PID_output = 0;
      agresive_temp_down = !agresive_temp_down;
    }
    else{
      agresive_temp_down = !agresive_temp_down;
    }
  }

  ledcWrite(pwm_channel, PID_output);
  Serial.println(String(ReadCelsius()) + " / " +  String(PID_output));
  current_time = millis();
}

void PreheatPlateStepOne() {
//   const int preheat_set_point = 35;
//   PID_input = _10klab::termo_k::ReadCelsius();
  setpoint = preheat_setpoint;
  Serial.println("preheat step 1");
  _10klab::digitalIO::turnOnAlarm(true);
  while (PID_input < preheat_setpoint) {
    PID_input = ReadCelsius();
    _10klab::screen::drawGraph(PID_input, setpoint);
    PIDControlAction();
  }
}

void PreheatPlateStepTwo() {
//   const unsigned long preheat_time = SeconToMillis(10);
  unsigned long start_time = 0;
  static unsigned long current_time = millis();
  Serial.println("preheat step 2");

  current_time = millis();
  start_time = millis();
  _10klab::digitalIO::turnOnAlarm(true);
  while (current_time < start_time + preheat_time) {
    PID_input = ReadCelsius();
    PIDControlAction();
    _10klab::screen::drawGraph(PID_input, setpoint);
    current_time = millis();
  }
}

void ReflowStepOne() {
//   const int preheat_set_point = 60;
//   PID_input = ReadCelsius();
  setpoint = heat_setpoint;
  Serial.println("reflow step 1");
  _10klab::digitalIO::turnOnAlarm(true);
  while (PID_input < heat_setpoint) {
    PID_input = ReadCelsius();
    _10klab::screen::drawGraph(PID_input, setpoint);
    PIDControlAction();
  }
}

void ReflowStepTwo() {
//   const unsigned long preheat_time = SeconToMillis(180);
  unsigned long start_time = 0;
  static unsigned long current_time = millis();
  Serial.println("reflow step 2");
  bool turn_off_alarm = false;

  current_time = millis();
  start_time = millis();
  while (current_time < start_time + heat_time) {
    PID_input = ReadCelsius();
    _10klab::screen::drawGraph(PID_input, setpoint);
    if(!turn_off_alarm){
      _10klab::digitalIO::turnOnAlarm(false);
      turn_off_alarm = _10klab::digitalIO::readButton(SELECT);
    }
    PIDControlAction();
    current_time = millis();
  }
}

void CoolingStep() {
  setpoint = 30;
  PID_input = ReadCelsius();
    Serial.println("cooling");


  // while (PID_input > setpoint) {
  //   PID_input = ReadCelsius();
  //   _10klab::screen::drawGraph(PID_input, setpoint);
  //   PIDControlAction();
  // }
}

void ProfilePIDProcess() {
  PreheatPlateStepOne();
  PreheatPlateStepTwo();
  ReflowStepOne();
  ReflowStepTwo();
  CoolingStep();
  // Serial.println("terminé");
}

void setupPIDProfile(int preheat_sp, int preheat_tm, int heat_sp, int heat_tm ){
    preheat_setpoint = preheat_sp;
    preheat_time = preheat_tm;
    heat_setpoint = heat_sp;
    heat_time = heat_tm;
    }

} // namespace termo_k
} // namespace _10klab