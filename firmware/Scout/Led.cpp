#include <Arduino.h>
#include "Led.h"

Led::Led(byte pin) : pin(pin) {
  pinMode(pin, OUTPUT);
}

bool Led::isOn() {
  return on;
}

void Led::setOn() {
  interval = LED_ON_INTERVAL;
}

void Led::setOff() {
  interval = LED_OFF_INTERVAL;
}

unsigned int Led::getInterval() {
  return interval;
}

void Led::setInterval(unsigned int interval) {
  this->interval = interval;
}

void Led::setFlash(unsigned int interval) {
  flash = true;
  this->interval = interval;
}

bool Led::isInvert() {
  return mod == 1;
}

void Led::setInvert(bool invert) {
  mod = invert ? 1 : 0;
}

void Led::update() {
  unsigned long now = millis();
  bool state;
  switch (interval) {
    case LED_OFF_INTERVAL:
      state = false;
      break;
    case LED_ON_INTERVAL:
      state = true;
      break;
    default:
      state = ((now / interval) % 2) == mod;
      if (flash && on && !state) {
        flash = false;
        interval = LED_OFF_INTERVAL;
      }
      break;
  }
  if (on == state) return;
  on = state;
  digitalWrite(pin, state ? HIGH : LOW);
}