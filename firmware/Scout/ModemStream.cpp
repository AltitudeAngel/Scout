#include "ModemStream.h"

void ModemStream::sendCmd(const __FlashStringHelper *cmd, const __FlashStringHelper *type = NULL) {
  // Send AT
  sendAT();

  // Send formatted command
  print(F("+"));
  print(cmd);
  if (type != NULL) print(type);
}

ModemStream::ModemStream(Stream *stream) : CaptureStream(stream) {
}

void ModemStream::sendAT() {
  // Flush any data and stop any capture
  flush();
  stopCapture();

  // Send a newline and AT
  sendNewline();
  print(F("AT"));
}

void ModemStream::sendNewline() {
  // Send newline
  print(F("\r\n"));
}

void ModemStream::sendTestCmd(const __FlashStringHelper *cmd) {
  sendCmd(cmd, F("=?"));
}

void ModemStream::sendReadCmd(const __FlashStringHelper *cmd) {
  sendCmd(cmd, F("?"));
}

void ModemStream::sendWriteCmd(const __FlashStringHelper *cmd) {
  sendCmd(cmd, F("="));
}

void ModemStream::sendExecCmd(const __FlashStringHelper *cmd) {
  sendCmd(cmd);
}

int ModemStream::recvCmdAck() {
  // Wait for a newline in response to a command
  return find(MODEM_NEWLINE, MODEM_NEWLINE_LEN) ? OK : ERR_NO_CMD_RESPONSE;
}

int ModemStream::recvOk() {
  // Wait for ok
  return find(MODEM_OK, MODEM_OK_LEN) ? OK : ERR_TIMEOUT;
}

int ModemStream::recvOkError() {
  // Wait for ok or error
  MultiTarget okError[] = {
    { MODEM_OK, MODEM_OK_LEN, 0 },
    { MODEM_ERROR, MODEM_ERROR_LEN, 0 }
  };
  int idx = findMulti(okError, 2);
  if (idx == -1) return ERR_TIMEOUT;
  return idx == 0 ? OK : ERR_ERROR_RESPONSE;
}

int ModemStream::recvDataStartError() {
  // Wait for data start or error
  MultiTarget dataStartError[] = {
    { MODEM_DATA_START, MODEM_DATA_START_LEN, 0 },
    { MODEM_ERROR, MODEM_ERROR_LEN, 0 }
  };
  int idx = findMulti(dataStartError, 2);
  if (idx == -1) return ERR_TIMEOUT;
  return idx == 0 ? OK : ERR_ERROR_RESPONSE;
}

int ModemStream::recvData(String *data) {
  // Start capturing into data
  startCapture(data);

  // Wait for an empty line
  bool emptyLine = find(MODEM_EMPTYLINE, MODEM_EMPTYLINE_LEN);
  stopCapture();
  if (!emptyLine) return ERR_TIMEOUT;

  // Remove empty line from data
  data->remove(data->length() - MODEM_EMPTYLINE_LEN, MODEM_EMPTYLINE_LEN);
  return OK;
}

int ModemStream::recvCmdData(const __FlashStringHelper *cmd, String *data) {
  // Format the prefix to look for from the cmd
  PGM_P p = reinterpret_cast<PGM_P>(cmd);
  size_t len = strlen_P(p);
  size_t prefixLen = len + 2;
  char prefix[prefixLen + 1];
  prefix[0] = '+';
  strncpy_P(prefix + 1, p, len);
  prefix[len + 1] = ':';
  prefix[prefixLen] = '\0';

  // Find the cmd prefix
  if (!find(prefix, prefixLen)) return ERR_TIMEOUT;

  // Start capturing the data
  startCapture(data);

  // Find the newline and stop the capture
  if (recvCmdAck() != OK) return ERR_NO_CMD_RESPONSE;
  stopCapture();

  // Remove the newline from the data and trim it
  data->remove(data->length() - MODEM_NEWLINE_LEN, MODEM_NEWLINE_LEN);
  data->trim();
  return OK;
}