#ifndef __POSITION_REPORT_H
#define __POSITION_REPORT_H

#include <WString.h>
#include <time.h>
#include <Stream.h>
#include <ArduinoJson.h>

#define MAX_POSITION_REPORT_SIZE 400
#define ISO_DATE_SIZE 21

const static char POSREP_SENSOR[] PROGMEM = "sensor";
const static char POSREP_ID[] PROGMEM = "id";
const static char POSREP_VOLTAGE[] PROGMEM = "voltage";
const static char POSREP_TEMPERATURE[] PROGMEM = "temperature";
const static char POSREP_RSSI[] PROGMEM = "rssi";
const static char POSREP_BER[] PROGMEM = "ber";
const static char POSREP_ADDITIONAL_INFO[] PROGMEM = "additionalInfo";
const static char POSREP_POSITIONS[] PROGMEM = "positions";
const static char POSREP_POSITION[] PROGMEM = "position";
const static char POSREP_TIMESTAMP[] PROGMEM = "sourceTimeStamp";
const static char POSREP_LAT[] PROGMEM = "lat";
const static char POSREP_LNG[] PROGMEM = "lng";
const static char POSREP_SOURCE[] PROGMEM = "source";
const static char POSREP_GPS[] PROGMEM = "gps";
const static char POSREP_ALTITUDES[] PROGMEM = "altitudes";
const static char POSREP_HEIGHT[] PROGMEM = "height";
const static char POSREP_DATUM[] PROGMEM = "datum";
const static char POSREP_WGS84[] PROGMEM = "wgs84";

class PositionReport {

  private:

    StaticJsonDocument<MAX_POSITION_REPORT_SIZE> report;

  public:

    PositionReport();

    void setSensorId(String id);
    void setId(uint32_t id);
    void setTimestamp(tm timestamp);
    void setPosition(double latitiude, double longitude);
    void setAltitude(double altitude);
    void setVoltage(float voltage);
    void setTemperature(int temperature);
    void setRssi(uint8_t rssi);
    void setBer(uint8_t ber);
    
    size_t length();
    size_t writeJson(Stream *stream);

};

#endif // __POSITION_REPORT_H