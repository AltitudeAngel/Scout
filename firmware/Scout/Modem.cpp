#include <Arduino.h>
#include "Modem.h"
#include "SessionStream.h"

Modem::Modem(ModemStream* stream) : stream(stream) {
}

ModemStream* Modem::getStream() {
  return stream;
}

bool Modem::getAlive() {
  stream->sendAT();
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setEcho(bool enable) {
  stream->sendAT();
  stream->print(F("E"));
  stream->print(enable ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getInfo(String &info) {
  stream->sendAT();
  stream->print(F("I"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvData(&info));
  RETURN_OK(stream->recvOkError());
}

bool Modem::setBaud(uint32_t baud) {
  stream->sendWriteCmd(F("IPR"));
  stream->print(baud);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setDefaults() {
  stream->sendAT();
  stream->print(F("&F"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setUserDefaults() {
  stream->sendAT();
  stream->print(F("Z0"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setFunctionality(uint8_t level) {
  stream->sendWriteCmd(F("CFUN"));
  stream->print(level);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setAutoTimeUpdate(bool enable) {
  stream->sendWriteCmd(F("CTZU"));
  stream->print(enable ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getHTPServers(String &servers) {
  stream->sendReadCmd(F("CHTPSERV"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CHTPSERV"), &servers));
  RETURN_OK(stream->recvOkError());
}

bool Modem::delHTPServer(uint8_t index) {
  stream->sendWriteCmd(F("CHTPSERV"));
  stream->print(F("\"DEL\","));
  stream->print(index);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::addHTPServer(const char *server, int port) {
  stream->sendWriteCmd(F("CHTPSERV"));
  stream->print(F("\"ADD\",\""));
  stream->print(server);
  stream->print(F("\","));
  stream->print(port);
  stream->print(F(",1"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getHTPState(String &state) {
  stream->sendReadCmd(F("CHTPUPDATE"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CHTPUPDATE"), &state));
  RETURN_OK(stream->recvOkError());
}

bool Modem::setTimeFromHTP() {
  stream->sendExecCmd(F("CHTPUPDATE"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getIMEI(String &imei) {
  stream->sendReadCmd(F("SIMEI"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("SIMEI"), &imei));
  RETURN_OK(stream->recvOkError());
}

bool Modem::getIMSI(String &imsi) {
  stream->sendExecCmd(F("CIMI"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvData(&imsi));
  RETURN_OK(stream->recvOkError());
}

bool Modem::getClock(String &clock) {
  stream->sendReadCmd(F("CCLK"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CCLK"), &clock));
  CHECK_OK(stream->recvOkError());
  clock.remove(0, 1);
  clock.remove(clock.length() - 1, 1);
  return true;
}

bool Modem::getClock(tm &time, long &tz) {
  String clock;
  if (!getClock(clock)) return false;
  time.tm_year = clock.substring(0, 2).toInt() + 100;
  time.tm_mon = clock.substring(3, 5).toInt() - 1;
  time.tm_mday = clock.substring(6, 8).toInt();
  time.tm_hour = clock.substring(9, 11).toInt();
  time.tm_min = clock.substring(12, 14).toInt();
  time.tm_sec = clock.substring(15, 17).toInt();
  tz = clock.substring(17, 20).toInt() * (15 * 60);
  return true;
}

bool Modem::getClock(time_t &time) {
  tm convert;
  long tz;
  if (!getClock(convert, tz)) return false;
  time = mktime(&convert) - tz;
  return true;
}

bool Modem::getVoltage(float &voltage) {
  stream->sendExecCmd(F("CBC"));
  stream->sendNewline();
  String data;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CBC"), &data));
  voltage = data.toFloat();
  RETURN_OK(stream->recvOkError());
}

bool Modem::getTemperature(int &temperature) {
  stream->sendExecCmd(F("CPMUTEMP"));
  stream->sendNewline();
  String data;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CPMUTEMP"), &data));
  temperature = data.toInt();
  RETURN_OK(stream->recvOkError());
}

bool Modem::getSignalQuality(uint8_t &rssi, uint8_t &ber) {
  stream->sendExecCmd(F("CSQ"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  String data;
  CHECK_OK(stream->recvCmdData(F("CSQ"), &data));
  int comma = data.indexOf(',');
  rssi = data.substring(0, comma).toInt();
  ber = data.substring(comma + 1).toInt();
  RETURN_OK(stream->recvOkError());
}

bool Modem::setGPSAntennaPower(bool enable) {
  stream->sendWriteCmd(F("CVAUXS"));
  stream->print(enable ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setGPSAntennaVoltage(unsigned int millivolts) {
  stream->sendWriteCmd(F("CVAUXV"));
  stream->print(millivolts);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getGPSAlive() {
  stream->sendReadCmd(F("CGPS"));
  stream->sendNewline();
  String data;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CGPS"), &data));
  CHECK_OK(stream->recvOkError());
  return data[0] == '1';
}

bool Modem::setGPSNMEAType(uint8_t type) {
  stream->sendWriteCmd(F("CGPSNMEA"));
  stream->print(type);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::startGPS() {
  stream->sendWriteCmd(F("CGPS"));
  stream->print(F("1,1"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::startGPSCold() {
  stream->sendExecCmd(F("CGPSCOLD"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::startGPSHot() {
  stream->sendExecCmd(F("CGPSHOT"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::stopGPS() {
  stream->sendWriteCmd(F("CGPS"));
  stream->print(F("0"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getGPSInfo(double &lat, double &lng, double &alt, double &speed, double &course) {
  stream->sendExecCmd(F("CGPSINFO"));
  stream->sendNewline();
  String gpsData;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CGPSINFO"), &gpsData));
  CHECK_OK(stream->recvOkError());
  const int totalCommas = 8;
  int commas[totalCommas];
  for (int pos = 0; pos < totalCommas; pos++) {
    commas[pos] = gpsData.indexOf(',', pos == 0 ? 0 : commas[pos - 1] + 1);
  }
  for (int pos = 0; pos <= totalCommas; pos++) {
    switch (pos) {
      case 0:
        // Latitude - ddmm.mmmmmm
        lat = gpsData.substring(0, 2).toDouble();
        lat += gpsData.substring(2, commas[pos]).toDouble() / 60.0;
        break;
      case 1:
        // North/South - S negative
        if (gpsData.substring(commas[pos - 1] + 1, commas[pos])[0] == 'S') lat *= -1.0;
        break;
      case 2:
        // Longitude - dddmm.mmmmmm
        lng = gpsData.substring(commas[pos - 1] + 1, commas[pos - 1] + 3).toDouble();
        lng += gpsData.substring(commas[pos - 1] + 3, commas[pos]).toDouble() / 60.0;
        break;
      case 3:
        // East/West - W negative
        if (gpsData.substring(commas[pos - 1] + 1, commas[pos])[0] == 'W') lng *= -1.0;
        break;
      case 4:
        // Date
        break;
      case 5:
        // Time
        break;
      case 6:
        // Altitude - meters
        alt = gpsData.substring(commas[pos - 1] + 1, commas[pos]).toDouble();
        break;
      case 7:
        // Speed - knots TODO convert to meters
        speed = gpsData.substring(commas[pos - 1] + 1, commas[pos]).toDouble();
        break;
      case 8:
        // Course - degrees
        course = gpsData.substring(commas[pos - 1] + 1, commas[pos]).toDouble();
        break;
    }
  }
  return true;
}

bool Modem::setNetworkProfile(uint8_t profile, bool activate) {
  stream->sendWriteCmd(F("CGACT"));
  stream->print(activate ? 1 : 0);
  stream->print(',');
  stream->print(profile);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setNetworkAPN(uint8_t profile, const char *apn, uint8_t dataCompress, uint8_t headerCompress) {
  stream->sendWriteCmd(F("CGDCONT"));
  stream->print(profile);
  stream->print(F(",\"IP\",\""));
  stream->print(apn);
  stream->print(F("\",\"0.0.0.0\","));
  stream->print(dataCompress);
  stream->print(F(","));
  stream->print(headerCompress);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setNetworkAuth(uint8_t profile, uint8_t authType, const char *username, const char *password) {
  stream->sendWriteCmd(F("CGAUTH"));
  stream->print(profile);
  stream->print(F(","));
  stream->print(authType);
  stream->print(F(",\""));
  stream->print(password);
  stream->print(F("\",\""));
  stream->print(username);
  stream->print(F("\""));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setNetworkMode(bool enableTransparent) {
  stream->sendWriteCmd(F("CIPMODE"));
  stream->print(enableTransparent ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setIPSendMode(bool enableAck) {
  stream->sendWriteCmd(F("CIPSENDMODE"));
  stream->print(enableAck ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getNetworkAlive(uint8_t &state) {
  stream->sendReadCmd(F("NETOPEN"));
  stream->sendNewline();
  String data;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("NETOPEN"), &data));
  CHECK_OK(stream->recvOkError());
  state = data.toInt();
  return true;
}

bool Modem::startNetwork() {
  stream->sendExecCmd(F("NETOPEN"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::stopNetwork() {
  stream->sendExecCmd(F("NETCLOSE"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getLocalIPAddress(String &ipAddress) {
  stream->sendExecCmd(F("IPADDR"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("IPADDR"), &ipAddress));
  RETURN_OK(stream->recvOkError());
}

bool Modem::getCommonChannelState(uint8_t &network, uint8_t &session0, uint8_t &session1) {
  stream->sendExecCmd(F("CCHSTATE"));
  stream->sendNewline();
  String data;
  CHECK_OK(stream->recvCmdAck());
  CHECK_OK(stream->recvCmdData(F("CCHSTATE"), &data));
  CHECK_OK(stream->recvOkError());
  network = data.substring(0, 1).toInt();
  session0 = data.substring(2, 3).toInt();
  session1 = data.substring(4, 5).toInt();
  return true;
}

bool Modem::setCommonChannelMode(bool transparent) {
  stream->sendWriteCmd(F("CCHMODE"));
  stream->print(transparent ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setCommonChannelParam(bool report, bool cache) {
  stream->sendWriteCmd(F("CCHSET"));
  stream->print(report ? 1 : 0);
  stream->print(F(","));
  stream->print(cache ? 1 : 0);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::startCommonChannel() {
  stream->sendExecCmd(F("CCHSTART"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::stopCommonChannel() {
  stream->sendExecCmd(F("CCHSTOP"));
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::setCommonChannelOpen(uint8_t session, const char *host, unsigned int port, uint8_t type) {
  stream->sendWriteCmd(F("CCHOPEN"));
  stream->print(session);
  stream->print(F(",\""));
  stream->print(host);
  stream->print(F("\","));
  stream->print(port);
  stream->print(F(","));
  stream->print(type);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

CaptureStream* Modem::getCommonChannelStream(uint8_t session) {
  if (session > 1) return NULL;
  if (sessionStreams[session] == NULL) {
    sessionStreams[session] = new CaptureStream(new SessionStream(session, this));
  }
  return sessionStreams[session];
}

bool Modem::setCommonChannelClose(uint8_t session) {
  stream->sendWriteCmd(F("CCHCLOSE"));
  stream->print(session);
  stream->sendNewline();
  CHECK_OK(stream->recvCmdAck());
  RETURN_OK(stream->recvOkError());
}

bool Modem::getCommonChannelSendBuffers(int &session0, int &session1) {
  stream->sendReadCmd(F("CCHSEND"));
  stream->sendNewline();
  String data = "";
  if (stream->recvCmdAck() != OK) return false;
  if (stream->recvCmdData(F("CCHSEND"), &data) != OK) return false;
  if (stream->recvOkError() != OK) return false;
  int midComma = data.indexOf(',', 2);
  session0 = data.substring(2, midComma).toInt();
  session1 = data.substring(midComma + 3).toInt();
  return true;
}

bool Modem::getCommonChannelRecvBuffers(int &session0, int &session1) {
  stream->sendReadCmd(F("CCHRECV"));
  stream->sendNewline();
  if (stream->recvCmdAck() != OK) return false;
  bool prefix;
  if (!recvCommonChannelDataEvent(prefix, session0, session1)) return false;
  if (!prefix) return false;
  if (stream->recvOkError() != OK) return false;
  return true;
}

bool Modem::sendCommonChannelData(uint8_t session, String *data) {
  stream->sendWriteCmd(F("CCHSEND"));
  stream->print(session);
  stream->print(F(","));
  stream->print(data->length());
  stream->sendNewline();
  if (stream->recvCmdAck() != OK) return false;
  if (stream->recvDataStartError() != OK) return false;
  stream->print(*data);
  if (stream->recvOk() != OK) return false;
  return true;
}

bool Modem::recvCommonChannelData(uint8_t session, String *data, int length, int &result) {
  // Initiate the receive
  stream->sendWriteCmd(F("CCHRECV"));
  stream->print(session);
  stream->print(F(","));
  stream->print(length);
  stream->sendNewline();
  if (stream->recvCmdAck() != OK) return false;
  if (stream->recvOkError() != OK) return false;

  // Receive data till we get to the end event
  bool dataToRead = true;
  do {
    bool prefix;
    int num1, num2;
    if (!recvCommonChannelDataEvent(prefix, num1, num2)) return false;
    if (num1 != session) continue;
    if (prefix) {
      while (num2 > 0) {
        int read = stream->read();
        if (read == -1) continue;
        data->concat((char)read);
        num2--;
      }
      continue;
    }
    dataToRead = false;
    result = num2;
  } while (dataToRead);
  return true;
}

bool Modem::recvCommonChannelDataEvent(bool &prefix, int &num1, int &num2) {
  String event;
  if (stream->recvCmdData(F("CCHRECV"), &event) != OK) return false;
  int comma1 = event.indexOf(',');
  int comma2 = event.indexOf(',', comma1 + 1);
  if (comma2 == -1) {
    prefix = false;
    num1 = event.substring(0, comma1).toInt();
    num2 = event.substring(comma1 + 1).toInt();
  } else {
    prefix = true;
    num1 = event.substring(comma1 + 1, comma2).toInt();
    num2 = event.substring(comma2 + 1).toInt();
  }
  return true;
}