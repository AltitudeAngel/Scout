#ifndef __HTTP_H
#define __HTTP_H

#include "CaptureStream.h"

#define HTTP_SPACE F(" ")
#define HTTP_CRLF F("\r\n")
#define HTTP_VERSION F("HTTP/1.1")
#define HTTP_HEADER_SEP F(": ")

#define HTTP_METHOD_POST F("POST")
#define HTTP_HEADER_HOST F("Host")
#define HTTP_HEADER_CONNECTION F("Connection")
#define HTTP_HEADER_CONNECTION_KEEP_ALVE F("keep-alive")
#define HTTP_HEADER_CONTENT_TYPE F("Content-Type")
#define HTTP_HEADER_CONTENT_LENGTH F("Content-Length")
#define HTTP_HEADER_USER_AGENT F("User-Agent")
#define HTTP_HEADER_AUTHORIZATION F("Authorization")

enum HttpState {
  REQUEST_LINE,
  REQUEST_HEADER,
  REQUEST_BODY,
  RESPONSE_LINE,
  RESPONSE_HEADER,
  RESPONSE_BODY
};

class Http {

  private:
    CaptureStream *stream;
    HttpState state = REQUEST_LINE;
    size_t sendEndLine();
    size_t sendHeaderStart(const __FlashStringHelper *name);
    bool recvSeparatedLine(const __FlashStringHelper *separator, String &before, String &after);

  public:
    Http(Stream *stream);

    HttpState getState();

    size_t sendRequestLine(const __FlashStringHelper *method, String uri);
    size_t sendRequestHeader(const __FlashStringHelper *name, const __FlashStringHelper *value);
    size_t sendRequestHeader(const __FlashStringHelper *name, String value);
    size_t sendRequestHeader(const __FlashStringHelper *name, String prefix, Stream *value);
    size_t sendRequestBody();
    bool sendRequestEnd();
  
    bool recvResponseLine(uint16_t &code, String &description);
    bool recvResponseHeader(String &name, String &value);
    bool recvResponseBody();
    bool recvResponseEnd();
};

#endif // __HTTP_H