#ifndef __LED_H
#define __LED_H

#define LED_OFF_INTERVAL 0
#define LED_ON_INTERVAL !0

class Led {
  private:
    byte pin;
    bool on = false;
    byte mod = 0;
    unsigned int interval = LED_OFF_INTERVAL;
    bool flash = false;

  public:
    Led(byte pin);
    bool isOn();
    void setOn();
    void setOff();
    unsigned int getInterval();
    void setInterval(unsigned int interval);
    void setFlash(unsigned int interval);
    bool isInvert();
    void setInvert(bool invert);
    void update();
};

#endif //__LED_H
