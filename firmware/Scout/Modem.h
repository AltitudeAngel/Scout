#ifndef __MODEM_H
#define __MODEM_H

#include <time.h>
#include "ModemStream.h"
#include "CaptureStream.h"

#define CHECK_OK(e) if (e != OK) return false;
#define RETURN_OK(e) return e == OK;

class Modem {
  friend class SessionStream;

  private:
    ModemStream *stream;
    CaptureStream *sessionStreams[2];
    bool recvCommonChannelDataEvent(bool &prefix, int &num1, int &num2);

  protected:
    ModemStream* getStream();

  public:
    Modem(ModemStream* stream);
    bool getAlive();
    bool setEcho(bool enable);
    bool getInfo(String &info);
    bool setBaud(uint32_t baud);
    bool setDefaults();
    bool setUserDefaults();
    bool setFunctionality(uint8_t level);
    bool setAutoTimeUpdate(bool enable);
    bool getHTPServers(String &servers);
    bool delHTPServer(uint8_t index);
    bool addHTPServer(const char *server, int port);
    bool getHTPState(String &state);
    bool setTimeFromHTP();
    bool getIMEI(String &imei);
    bool getIMSI(String &imsi);
    bool getClock(String &clock);
    bool getClock(tm &time, long &tz);
    bool getClock(time_t &time);
    bool getVoltage(float &voltage);
    bool getTemperature(int &temperature);
    bool getSignalQuality(uint8_t &rssi, uint8_t &ber);
    bool setGPSAntennaPower(bool enable);
    bool setGPSAntennaVoltage(unsigned int millivolts);
    bool getGPSAlive();
    bool setGPSNMEAType(uint8_t type);
    bool startGPS();
    bool startGPSCold();
    bool startGPSHot();
    bool stopGPS();
    bool getGPSInfo(double &lat, double &lng, double &alt, double &speed, double &course);
    bool setNetworkProfile(uint8_t profile, bool activate);
    bool setNetworkAPN(uint8_t profile, const char *apn, uint8_t dataCompress, uint8_t headerCompress);
    bool setNetworkAuth(uint8_t profile, uint8_t authType, const char *username, const char *password);
    bool setNetworkMode(bool enableTransparent);
    bool setIPSendMode(bool enableAck);
    bool getNetworkAlive(uint8_t &state);
    bool startNetwork();
    bool stopNetwork();
    bool getLocalIPAddress(String &ipAddress);
    bool getCommonChannelState(uint8_t &network, uint8_t &session0, uint8_t &session1);
    bool setCommonChannelMode(bool transparent);
    bool setCommonChannelParam(bool report, bool cache);
    bool startCommonChannel();
    bool stopCommonChannel();
    bool setCommonChannelOpen(uint8_t session, const char *host, unsigned int port, uint8_t type);
    CaptureStream* getCommonChannelStream(uint8_t session);
    bool setCommonChannelClose(uint8_t session);
    bool getCommonChannelSendBuffers(int &session0, int &session1);
    bool getCommonChannelRecvBuffers(int &session0, int &session1);
    bool sendCommonChannelData(uint8_t session, String *data);
    bool recvCommonChannelData(uint8_t session, String *data, int length, int &result);
};

#endif // __MODEM_H