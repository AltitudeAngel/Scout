#include <time.h>
#include "PositionReport.h"

PositionReport::PositionReport() {
}

void PositionReport::setSensorId(String id) {
  report[POSREP_SENSOR][POSREP_ID] = id;
}

void PositionReport::setId(uint32_t id) {
  report[POSREP_POSITIONS][0][POSREP_POSITION][POSREP_ID] = String(id);
}

void PositionReport::setTimestamp(tm timestamp) {
  String iso;
  iso.concat(timestamp.tm_year + 1900);
  iso.concat('-');
  iso.concat(timestamp.tm_mon + 1);
  iso.concat('-');
  iso.concat(timestamp.tm_mday);
  iso.concat('T');
  iso.concat(timestamp.tm_hour);
  iso.concat(':');
  iso.concat(timestamp.tm_min);
  iso.concat(':');
  iso.concat(timestamp.tm_sec);
  iso.concat('Z');
  report[POSREP_POSITIONS][0][POSREP_POSITION][POSREP_TIMESTAMP] = iso;
}

void PositionReport::setPosition(double latitiude, double longitude){
  report[POSREP_POSITIONS][0][POSREP_POSITION][POSREP_LAT] = latitiude;
  report[POSREP_POSITIONS][0][POSREP_POSITION][POSREP_LNG] = longitude;
  report[POSREP_POSITIONS][0][POSREP_POSITION][POSREP_SOURCE] = POSREP_GPS;
}

void PositionReport::setAltitude(double altitude){
  report[POSREP_POSITIONS][0][POSREP_ALTITUDES][0][POSREP_HEIGHT] = altitude;
  report[POSREP_POSITIONS][0][POSREP_ALTITUDES][0][POSREP_DATUM] = POSREP_WGS84;
  report[POSREP_POSITIONS][0][POSREP_ALTITUDES][0][POSREP_SOURCE] = POSREP_GPS;
}

void PositionReport::setVoltage(float voltage){
  report[POSREP_SENSOR][POSREP_ADDITIONAL_INFO][POSREP_VOLTAGE] = voltage;
}

void PositionReport::setTemperature(int temperature){
  report[POSREP_SENSOR][POSREP_ADDITIONAL_INFO][POSREP_TEMPERATURE] = temperature;
}

void PositionReport::setRssi(uint8_t rssi){
  report[POSREP_SENSOR][POSREP_ADDITIONAL_INFO][POSREP_RSSI] = rssi;
}

void PositionReport::setBer(uint8_t ber){
  report[POSREP_SENSOR][POSREP_ADDITIONAL_INFO][POSREP_BER] = ber;
}

size_t PositionReport::length() {
  return measureJson(report);
}

size_t PositionReport::writeJson(Stream * stream) {
  return serializeJson(report, *stream);
}