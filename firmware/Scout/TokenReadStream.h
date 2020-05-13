#ifndef __TOKEN_READ_STREAM_H
#define __TOKEN_READ_STREAM_H

#include <Stream.h>

class TokenReadStream : public Stream {

  private:
    int pos;

  public:
    TokenReadStream(int startOffset);
    virtual int peek();
    virtual int read();
    virtual int available();
    virtual size_t write(uint8_t data);
    virtual void flush();

};

#endif // __TOKEN_READ_STREAM_H