#include "CaptureStream.h"

void CaptureStream::captureString(uint8_t read, void* state) {
  String *capture = (String*)state;
  capture->concat((char)read);
}

CaptureStream::CaptureStream(Stream *stream) : stream(stream) {
  setTimeout(stream->getTimeout());
}

void CaptureStream::startCapture(void (*capture)(uint8_t,void*), void *state) {
  this->capture = capture;
  this->state = state;
}

void CaptureStream::startCapture(String *capture) {
  this->capture = &captureString;
  this->state = capture;
}

void CaptureStream::stopCapture() {
  capture = NULL;
  state = NULL;
}

int CaptureStream::peek() {
  return stream->peek();
}

int CaptureStream::read() {
  int read = stream->read();
  if (read == -1) return read;
  if (capture != NULL) capture((uint8_t)read, state);
  return read;
}

int CaptureStream::available() {
  return stream->available();
}

size_t CaptureStream::write(uint8_t data) {
  return stream->write(data);
}

void CaptureStream::flush() {
  stream->flush();
}