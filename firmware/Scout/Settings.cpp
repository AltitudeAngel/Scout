#include <EEPROM.h>
#include <time.h>
#include "Settings.h"
#include "CaptureStream.h"
#include "TokenReadStream.h"

void Settings::captureToEEPROM(uint8_t read, void* state) {
  int *idx = (int*)state;
  if (*idx > OFFSET_MAX) return;
  if (read == '"') read = '\0';
  EEPROM.update((*idx)++, read);
}

Settings::Settings() {
}

time_t Settings::getTokenExpire() {
  time_t expire;
  EEPROM.get(OFFSET_TOKEN_EXPIRE, expire);
  return expire;
}

void Settings::setTokenExpire(time_t expiry) {
  EEPROM.put(OFFSET_TOKEN_EXPIRE, expiry);
}

int Settings::setToken(Stream *stream) {
  // Wrap in CaptureStream
  CaptureStream capture(stream);

  // Find opening double quote
  if (!capture.find('"')) return 0;

  // Capture stream to EEPROM
  writeIdx = OFFSET_TOKEN;
  capture.startCapture(captureToEEPROM, &writeIdx);

  // Find closing double quote
  if (!capture.find('"')) return 0;
  capture.stopCapture();

  // Return bytes written
  return writeIdx - OFFSET_TOKEN;
}

Stream* Settings::getToken() {
  return new TokenReadStream(OFFSET_TOKEN);
}

bool Settings::checkToken() {
  // Check first few characters of token are printable
  uint8_t pos = OFFSET_TOKEN;
  while (pos <= CHECK_MAX) {
    uint8_t next = EEPROM.read(pos++);
    if (next < ' ' || next > '~') return false;
  }
  return true;
}