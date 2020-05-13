#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <Stream.h>

#define OFFSET_MAX 1000
#define OFFSET_TOKEN_EXPIRE 0
#define OFFSET_TOKEN OFFSET_TOKEN_EXPIRE + sizeof(time_t)
#define CHECK_MAX OFFSET_TOKEN + 20

class Settings {

  private:
    int writeIdx;

    static void captureToEEPROM(uint8_t read, void* state);

  public:
    Settings();
    time_t getTokenExpire();
    void setTokenExpire(time_t expiry);
    int setToken(Stream *stream);
    Stream* getToken();
    bool checkToken();
};

static Settings SETTINGS;

#endif // __SETTINGS_H