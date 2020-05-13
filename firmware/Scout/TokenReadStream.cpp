#include <EEPROM.h>
#include "TokenReadStream.h"

TokenReadStream::TokenReadStream(int startOffset) : pos(startOffset) {
}

int TokenReadStream::peek() {
  uint8_t next = EEPROM.read(pos);
  if (next == '\0') return -1;
  return next;
}

int TokenReadStream::read() {
  uint8_t next = EEPROM.read(pos);
  if (next == '\0') return -1;
  pos++;
  return next;
}

int TokenReadStream::available() {
  uint8_t next = EEPROM.read(pos);
  if (next == '\0') return 0;
  return 1;
}

size_t TokenReadStream::write(uint8_t) {
  // Not implemented
  return 0;
}

void TokenReadStream::flush() {
  // Do nothing
}