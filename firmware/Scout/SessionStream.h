#ifndef __SessionStream_H
#define __SessionStream_H

#include "Modem.h"

#define CHECK_INTERVAL 100
#define MAX_SEND_BUFFER 100
#define MAX_RECV_BUFFER 100
#define TIMEOUT_RETRIES 10

class SessionStream : public Stream {
  private:
    byte session;
    Modem *modem;
    String sendBuf = "";
    String recvBuf = "";

    bool recvBuffer();
    bool sendBuffer();    

  public:
    SessionStream(byte session, Modem *modem) ;
    virtual int peek();
    virtual int read();
    virtual int available();
    virtual size_t write(uint8_t data);
    virtual void flush();
};

#endif //__SessionStream_H
