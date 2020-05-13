#include "Http.h"
#include <Arduino.h>

Http::Http(Stream *stream) : stream(new CaptureStream(stream)) {
}

size_t Http::sendEndLine() {
  return stream->print(HTTP_CRLF);
}

size_t Http::sendHeaderStart(const __FlashStringHelper *name) {
  size_t sent = 0;
  sent += stream->print(name);
  sent += stream->print(HTTP_HEADER_SEP);
  return sent;
}

bool Http::recvSeparatedLine(const __FlashStringHelper *separator, String &before, String &after) {
  String sep(separator);
  String crlf(HTTP_CRLF);
  before = "";
  after = "";
  stream->startCapture(&before);
  bool found = stream->findUntil((char*)sep.c_str(), sep.length(), (char*)crlf.c_str(), crlf.length());
  stream->stopCapture();
  if (!found) {
    before = "";
    return false;
  }
  before.remove(before.length() - sep.length(), sep.length());
  stream->startCapture(&after);
  found = stream->find((char*)crlf.c_str(), crlf.length());
  stream->stopCapture();
  if (!found) {
    before = "";
    after = "";
    return false;
  }
  after.remove(after.length() - crlf.length(), crlf.length());
  return true;
}

HttpState Http::getState() {
  return state;
}

size_t Http::sendRequestLine(const __FlashStringHelper *method, String uri) {
  if (state != REQUEST_LINE) return 0;
  size_t sent = 0;
  sent += stream->print(method);
  sent += stream->print(HTTP_SPACE);
  sent += stream->print(uri);
  sent += stream->print(HTTP_SPACE);
  sent += stream->print(HTTP_VERSION);
  sent += sendEndLine();
  state = REQUEST_HEADER;
  return sent;
}

size_t Http::sendRequestHeader(const __FlashStringHelper *name,const  __FlashStringHelper *value) {
  if (state != REQUEST_HEADER) return 0;
  size_t sent = 0;
  sent += sendHeaderStart(name);
  sent += stream->print(value);
  sent += sendEndLine();
  return sent;
}

size_t Http::sendRequestHeader(const __FlashStringHelper *name, String value) {
  if (state != REQUEST_HEADER) return 0;
  size_t sent = 0;
  sent += sendHeaderStart(name);
  sent += stream->print(value);
  sent += sendEndLine();
  return sent;
}

size_t Http::sendRequestHeader(const __FlashStringHelper *name, String prefix, Stream *value) {
  if (state != REQUEST_HEADER) return 0;
  size_t sent = 0;
  sent += sendHeaderStart(name);
  if (prefix != NULL) {
    sent += stream->print(prefix);
  }
  while (true) {
    int read = value->read();
    if (read == -1) break;
    sent += stream->write((uint8_t)read);
  }
  sent += sendEndLine();
  return sent;
}

size_t Http::sendRequestBody() {
  if (state != REQUEST_HEADER) return 0;
  size_t sent = 0;
  sent += sendEndLine();
  state = REQUEST_BODY;
  return sent;
}

bool Http::sendRequestEnd() {
  if (state != REQUEST_BODY) return false;
  state = RESPONSE_LINE;
  return true;
}

bool Http::recvResponseLine(uint16_t &code, String &description) {
  if (state != RESPONSE_LINE) return false;
  String version(HTTP_VERSION);
  if (!stream->find((char*)version.c_str(), version.length())) return false;
  if (stream->read() != ' ') return false;
  String codeNum;
  if (!recvSeparatedLine(HTTP_SPACE, codeNum, description)) return false;
  code = codeNum.toInt();
  state = RESPONSE_HEADER;
  return true;
}

bool Http::recvResponseHeader(String &name, String &value) {
  if (state != RESPONSE_HEADER) return false;
  if (!recvSeparatedLine(HTTP_HEADER_SEP, name, value)) {
    state = RESPONSE_BODY;
    return false;
  }
  return true;
}

bool Http::recvResponseBody() {
  if (state != RESPONSE_BODY) return false;
  return true;
}

bool Http::recvResponseEnd() {
  if (state != RESPONSE_BODY) return false;
  state = REQUEST_LINE;
  return true;
}