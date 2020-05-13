#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "ModemStream.h"
#include "Modem.h"
#include "Led.h"
#include "Button.h"

// Pins
#define PIN_MODEM_TX 0
#define PIN_MODEM_RX 1
#define PIN_MODEM_PWR 2
#define PIN_MODEM_SLP 3
#define PIN_BTN_PAIR 6
#define PIN_LED_PWR 7
#define PIN_LED_PAIR 8

// Modem
#define MODEM_SERIAL Serial
#define MODEM_TIMEOUT 1000
#define MODEM_RETRIES 60
#define MODEM_DEFAULT_BAUD 115200L
ModemStream modemStream(&MODEM_SERIAL);
Modem modem(&modemStream);

// LEDs
#define LED_FLASH_SLOW 500
#define LED_FLASH_MEDIUM 250
#define LED_FLASH_FAST 100
#define LED_FLASH_PING 50
Led powerLed(PIN_LED_PWR);
Led pairLed(PIN_LED_PAIR);

// Buttons
#define BTN_SHORT_PRESS 200
#define BTN_LONG_PRESS 2000
Button pairButton(PIN_BTN_PAIR);

#endif //__HARDWARE_H