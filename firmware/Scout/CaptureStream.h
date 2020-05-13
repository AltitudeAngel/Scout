#ifndef __CAPTURE_STREAM_H
#define __CAPTURE_STREAM_H

#include <Stream.h>

class CaptureStream : public Stream {

  private:
    Stream *stream;
    void (*capture)(uint8_t,void*) = NULL;
    void *state = NULL;
    static void captureString(uint8_t read, void* state);
  
  public:
    CaptureStream(Stream *stream);
    void startCapture(void (*capture)(uint8_t,void*), void *state);
    void startCapture(String *capture);
    void stopCapture();
    virtual int peek();
    virtual int read();
    virtual int available();
    virtual size_t write(uint8_t data);
    virtual void flush();
};

#endif // __CAPTURE_STREAM_H