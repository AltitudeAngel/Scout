#ifndef __MODEM_STREAM_H
#define __MODEM_STREAM_H

#include "CaptureStream.h"

static char MODEM_NEWLINE[] = "\r\n";
static size_t MODEM_NEWLINE_LEN = strlen(MODEM_NEWLINE);
static char MODEM_OK[] = "OK\r\n";
static size_t MODEM_OK_LEN = strlen(MODEM_OK);
static char MODEM_ERROR[] = "ERROR\r\n";
static size_t MODEM_ERROR_LEN = strlen(MODEM_ERROR);
static char MODEM_EMPTYLINE[] = "\r\n\r\n";
static size_t MODEM_EMPTYLINE_LEN = strlen(MODEM_EMPTYLINE);
static char MODEM_DATA_START[] = ">";
static size_t MODEM_DATA_START_LEN = strlen(MODEM_DATA_START);

#define OK 0
#define ERR_TIMEOUT -1
#define ERR_NO_CMD_RESPONSE -2
#define ERR_ERROR_RESPONSE -3

class ModemStream : public CaptureStream {

  private:

    void sendCmd(const __FlashStringHelper *cmd, const __FlashStringHelper *type);

  public:

    ModemStream(Stream *stream);
    void sendAT();
    void sendNewline();
    void sendTestCmd(const __FlashStringHelper *cmd);
    void sendReadCmd(const __FlashStringHelper *cmd);
    void sendWriteCmd(const __FlashStringHelper *cmd);
    void sendExecCmd(const __FlashStringHelper *cmd);
    int recvCmdAck();
    int recvOk();
    int recvOkError();
    int recvDataStartError();
    int recvData(String *data);
    int recvCmdData(const __FlashStringHelper *cmd, String *data);
};

#endif // __MODEM_STREAM_H