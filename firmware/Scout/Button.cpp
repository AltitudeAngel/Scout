#include <Arduino.h>
#include "Button.h"

Button::Button(byte pin) : pin(pin) {
  pinMode(pin, INPUT_PULLUP);
}

bool Button::isNotPressed() {
  unsigned int interval = millis() - start;
  return !pressed || interval < shortPress;
}

bool Button::isShortPressed() {
  unsigned int interval = millis() - start;
  return pressed && interval >= shortPress && interval < longPress;
}

bool Button::isLongPressed() {
  unsigned int interval = millis() - start;
  return pressed && interval >= longPress;
}

void Button::setShortPress(unsigned int shortPress) {
  this->shortPress = shortPress;
}

void Button::setLongPress(unsigned int longPress) {
  this->longPress = longPress;
}

void Button::update() {
  bool state = digitalRead(pin) == LOW;
  if (pressed == state) return;
  start = millis();
  pressed = state;
}