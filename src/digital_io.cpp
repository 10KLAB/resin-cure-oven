#include "digital_io.h"
#include "screen_control.h"
#include "termo_k.h"

namespace _10klab {
namespace digitalIO {
bool next_button = false;
bool back_button = false;
bool select_button = false;

void IRAM_ATTR nextButton() {
  next_button = true;
  //   detachInterrupt(NEXT);
}
void IRAM_ATTR backButton() {
  //   detachInterrupt(BACK);
  back_button = true;
}
void IRAM_ATTR selectButton() {
  // detachInterrupt(SELECT);
  select_button = true;
}

void setupIO() {
  pinMode(BACK, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(ALARM, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BACK), backButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(NEXT), nextButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(SELECT), selectButton, FALLING);
}

void turnOnAlarm(bool one_time) {
  static unsigned long current_time = 0;
  const int wait_time = 3000;
  const int pulse_time = 100;

  if (one_time) {
    current_time = 0;
  }

  if (millis() >= current_time + wait_time) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(ALARM, HIGH);
      delay(pulse_time);
      digitalWrite(ALARM, LOW);
      delay(pulse_time);
    }
    current_time = millis();
  }
}

bool verifyButton(int button) {
  delay(50);
  if (!digitalRead(button)) {
    return true;
  }
  return false;
}

bool readButton(int button) {
  // const int debounce_delay = 100;
  static unsigned long last_debounce_time = 0;
  bool get_state = false;
  static bool enable_interrupts = false;

  // if (millis() < last_debounce_time + debounce_delay) {
  //   back_button = false;
  //   next_button = false;
  //   select_button = false;
  //   return false;
  // }

  if (button == BACK && back_button) {
    // detachInterrupt(BACK);
    if (verifyButton(BACK)) {
      get_state = true;
    } else {
      get_state = false;
    }
    // get_state = back_button;
    back_button = false;
    enable_interrupts = true;
    last_debounce_time = millis();
    return get_state;
  }

  else if (button == NEXT && next_button) {
    // detachInterrupt(NEXT);
    if (verifyButton(NEXT)) {
      get_state = true;
    } else {
      get_state = false;
    }
    // get_state = next_button;
    next_button = false;
    enable_interrupts = true;
    last_debounce_time = millis();
    return get_state;
  } else if (button == SELECT && select_button) {
    // detachInterrupt(SELECT);
    if (verifyButton(SELECT)) {
      get_state = true;
    } else {
      get_state = false;
    }
    // get_state = select_button;
    select_button = false;
    enable_interrupts = true;
    last_debounce_time = millis();
    return get_state;
  }
  // if (enable_interrupts) {
  // enable_interrupts = false;
  // attachInterrupt(digitalPinToInterrupt(NEXT), nextButton, RISING);
  // attachInterrupt(digitalPinToInterrupt(BACK), backButton, RISING);
  // attachInterrupt(digitalPinToInterrupt(SELECT), selectButton, RISING);
  // }
  return get_state;
}

unsigned long minutsToMillis(int minuts) { return minuts * 1000 * 60; }

void selectPIDParameters() {
  int preheat_time = 5;
  int heat_time = 30;
  int preheat_setpoint = 40;
  int heat_setpoint = 60;

  int previous_value = 0;
  bool end_setup = false;

  while (!end_setup) {

    _10klab::screen::ShowLineOne("Pick PH-T");
    _10klab::screen::ShowLineTwo(String(preheat_time) + " min");
    while (!readButton(SELECT)) {
      if (readButton(NEXT)) {
        preheat_time++;
      }
      if (readButton(BACK)) {
        preheat_time--;
        if (preheat_time <= 1) {
          preheat_time = 1;
        }
      }

      if (preheat_time != previous_value) {
        _10klab::screen::ShowLineOne("Pick PH-T");
        _10klab::screen::ShowLineTwo(String(preheat_time) + " min");
        previous_value = preheat_time;
      }
    }
    delay(500);

    _10klab::screen::ShowLineOne("Pick PH-SP");
    _10klab::screen::ShowLineTwo(String(preheat_setpoint) + " C");
    while (!readButton(SELECT)) {
      if (readButton(NEXT)) {
        preheat_setpoint++;
        if (preheat_setpoint >= 85) {
          preheat_setpoint = 85;
        }
      }
      if (readButton(BACK)) {
        preheat_setpoint--;
        if (preheat_setpoint <= 1) {
          preheat_setpoint = 1;
        }
      }

      if (preheat_setpoint != previous_value) {
        _10klab::screen::ShowLineOne("Pick PH-SP");
        _10klab::screen::ShowLineTwo(String(preheat_setpoint) + " C");
        previous_value = preheat_setpoint;
      }
    }
    delay(500);
    /////////////////////////////////////////////////////////////////////////////////////

    _10klab::screen::ShowLineOne("Pick H-T");
    _10klab::screen::ShowLineTwo(String(heat_time) + " min");
    while (!readButton(SELECT)) {
      if (readButton(NEXT)) {
        heat_time++;
      }
      if (readButton(BACK)) {
        heat_time--;
        if (heat_time <= 1) {
          heat_time = 1;
        }
      }

      if (heat_time != previous_value) {
        _10klab::screen::ShowLineOne("Pick H-T");
        _10klab::screen::ShowLineTwo(String(heat_time) + " min");
        previous_value = heat_time;
      }
    }
    delay(500);

    _10klab::screen::ShowLineOne("Pick H-SP");
    _10klab::screen::ShowLineTwo(String(heat_setpoint) + " C");
    while (!readButton(SELECT)) {
      if (readButton(NEXT)) {
        heat_setpoint++;
        if (heat_setpoint >= 85) {
          heat_setpoint = 85;
        }
      }
      if (readButton(BACK)) {
        heat_setpoint--;
        if (heat_setpoint <= 1) {
          heat_setpoint = 1;
        }
      }

      if (heat_setpoint != previous_value) {
        _10klab::screen::ShowLineOne("Pick H-SP");
        _10klab::screen::ShowLineTwo(String(heat_setpoint) + " C");
        previous_value = heat_setpoint;
      }
    }
    delay(500);

    _10klab::screen::ShowLineOne("End setup?");
    _10klab::screen::ShowLineTwo("yes/no");
    bool end_menu = false;
    while (!end_menu) {
      if (readButton(BACK)) {
        end_menu = true;
        end_setup = true;
      }
      if (readButton(NEXT)) {
        end_menu = true;
      }
    }
    delay(500);

    _10klab::termo_k::setupPIDProfile(preheat_setpoint,
                                      minutsToMillis(preheat_time),
                                      heat_setpoint, minutsToMillis(heat_time));
  }
}

} // namespace digitalIO
} // namespace _10klab
