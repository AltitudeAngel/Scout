#ifndef __ERROR_H
#define __ERROR_H

#include "Hardware.h"

#define FATAL(m) stop(F(m), LED_FLASH_SLOW)
#define END() stop(F("END"), LED_FLASH_FAST)

#define RETRY(e, m) \
  { \
    int retries = 0; \
    while (true) { \
      if (e) break; \
      if (++retries >= MODEM_RETRIES) FATAL(m); \
      delay(MODEM_TIMEOUT); \
    } \
  }

void stop(const __FlashStringHelper* message, unsigned int flash) {
  // Print message to modem serial for debugging
  MODEM_SERIAL.print(F("# STOP: "));
  MODEM_SERIAL.print(message);
  
  // Indicate a stop by alternate flashing both LEDs
  pairLed.setInvert(false);
  powerLed.setInterval(flash);
  pairLed.setInvert(true);
  pairLed.setInterval(flash);

  // Infinite loop
  while (true) {
    delay(MODEM_TIMEOUT);
  }
}

#endif //__ERROR_H
