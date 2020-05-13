#ifndef __BUTTON_H
#define __BUTTON_H

#define BTN_DEFAULT_SHORT_PRESS 100
#define BTN_DEFAULT_LONG_PRESS 1000

class Button {
  private:
    byte pin;
    bool pressed;
    unsigned long start = 0;
    unsigned int shortPress = BTN_DEFAULT_SHORT_PRESS;
    unsigned int longPress = BTN_DEFAULT_LONG_PRESS;

  public:
    Button(byte pin);
    bool isNotPressed();
    bool isShortPressed();
    bool isLongPressed();
    void setShortPress(unsigned int shortPress);
    void setLongPress(unsigned int longPress);
    void update();
};

#endif //__BUTTON_H
