#include <Arduino.h>
#include <avr/pgmspace.h>
#include <time.h>

// #define USE_TEST_METHODS
// #define USE_POWER_OFF_FIRST
// #define USE_POWER_ON
// #define USE_ACTIVE_GPS_ANTENNA
// #define ACTIVE_GPS_ANTENNA_MILLIVOLTS 3050
#define USE_HTP_TIME_SYNC

#include "Error.h"
#include "Hardware.h"
#include "Settings.h"
#include "Http.h"
#include "PositionReport.h"

#define MODEM_APN "giffgaff.com"
#define MODEM_USERNAME "giffgaff"
#define MODEM_PASSWORD ""

#define POSITION_INTERVAL 1000
#define PAIR_INTERVAL 1000
#define PAIR_TIMEOUT 60000L
#define PAIR_EXPIRY_MIN_SEC 60L * 60L
#define PAIR_EXPIRY_MAX_SEC 24L * 60L * 60L
#define HTTP_OK 200
#define HTTP_ACCEPTED 202
#define HTTP_UNAUTHORIZED 401
#define HTTP_NOT_FOUND 404
#define MAX_FAILED_AUTH 3
#define SCOUT_USER_AGENT F("AltitudeAngelScout/1.0.0")
#define MIME_CONTENT_TYPE_JSON F("application/json")
#define AUTHORIZATION_BEARER F("Bearer ")
#define SURVEILLANCE_API "surveillance-api.altitudeangel.com"
#define PAIR_URI_PREFIX F("/v1/pairing-tokens/consume-token?clientUniqueId=imei:")
#define REPORT_URI F("/v1/position-reports")

static char ACCESS_TOKEN_JSON[] = "\"accessToken\":";
static size_t ACCESS_TOKEN_JSON_LEN = strlen(ACCESS_TOKEN_JSON);
static char EXPIRES_AT_JSON[] = "\"expiresAt\":";
static size_t EXPIRES_AT_JSON_LEN = strlen(EXPIRES_AT_JSON);

unsigned long reportId = 0;

void setup() {
  // Hook timer 0 to manage LED flashing and button presses
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  // Set up pin modes not covered by LEDs or Buttons
  pinMode(PIN_MODEM_TX, INPUT);
  pinMode(PIN_MODEM_RX, OUTPUT);
  pinMode(PIN_MODEM_PWR, OUTPUT);
  pinMode(PIN_MODEM_SLP, OUTPUT);

  // Set up button press durations
  pairButton.setShortPress(BTN_SHORT_PRESS);
  pairButton.setLongPress(BTN_LONG_PRESS);

  // Start serial comms to modem at default baud and set timeout
  MODEM_SERIAL.begin(MODEM_DEFAULT_BAUD);
  MODEM_SERIAL.setTimeout(MODEM_TIMEOUT);
  modemStream.setTimeout(MODEM_TIMEOUT);
  do {
    delay(1);
  } while (!MODEM_SERIAL);
  delay(100);
  MODEM_SERIAL.flush();

  // SLP should stay LOW whilst on
  digitalWrite(PIN_MODEM_SLP, LOW);

#ifdef USE_POWER_OFF_FIRST
  // Off = LOW for >= 2500ms then HIGH
  digitalWrite(PIN_MODEM_PWR, HIGH);
  delay(1000);
  digitalWrite(PIN_MODEM_PWR, LOW);
  delay(2600);
#endif
#ifdef USE_POWER_ON
  // On = LOW for >= 500ms then HIGH
  digitalWrite(PIN_MODEM_PWR, HIGH);
  delay(1000);
  digitalWrite(PIN_MODEM_PWR, LOW);
  delay(600);
#endif

  // PWR should stay HIGH whilst on
  digitalWrite(PIN_MODEM_PWR, HIGH);
}

SIGNAL(TIMER0_COMPA_vect)
{
  // Update the LED states
  powerLed.update();
  pairLed.update();

  // Update the button states
  pairButton.update();
}

void loop() {
  // Start the power LED flashing at a slow rate to indicate loop start
  powerLed.setInterval(LED_FLASH_SLOW);
  pairLed.setOff();

  // Try to get serial communications with the modem
  RETRY(modem.getAlive(), "MODEM");

  // Serial communication established so flash the power LED medium
  powerLed.setInterval(LED_FLASH_MEDIUM);

  // Ensure modem is using default settings
  RETRY(modem.setDefaults(), "DEFAULT");
  RETRY(modem.setUserDefaults(), "DEFAULT");

  // Set echo off
  RETRY(modem.setEcho(false), "ECHO");

  // Ensure modem is in full functionality
  RETRY(modem.setFunctionality(1), "FUNC");

  // Get the modem IMEI
  String imei;
  RETRY(modem.getIMEI(imei), "IMEI");

#ifdef USE_TEST_METHODS
  // Get the modem IMSI
  String imsi;
  RETRY(modem.getIMSI(imsi), "IMSI");
#endif

#ifdef USE_TEST_METHODS
  // Get the modem info
  String info;
  RETRY(modem.getInfo(info), "INFO");
#endif

  // Set automatic time updates
  RETRY(modem.setAutoTimeUpdate(true), "TIME");

  // Configure the network
  byte profile = 1;
  RETRY(modem.setNetworkAPN(profile, MODEM_APN, 0, 0), "APN");
  RETRY(modem.setNetworkAuth(profile, 1, MODEM_USERNAME, MODEM_PASSWORD), "AUTH");
  RETRY(modem.setNetworkProfile(profile, true), "PROON");

  // Ensure the network is stopped
  byte netstate;
  RETRY(modem.getNetworkAlive(netstate), "GETNET");
  if (netstate != 0) {
    RETRY(modem.stopNetwork(), "STOPNET");
  }

  // Start the network
  RETRY(modem.setNetworkMode(false), "SETMODE");
  RETRY(modem.setIPSendMode(true), "SETSEND");
  RETRY(modem.startNetwork(), "STARTNET");
  RETRY(modem.getNetworkAlive(netstate) && netstate == 1, "NET");
  
#ifdef USE_TEST_METHODS
  // Get the local IP address
  String ipAddress;
  RETRY(modem.getLocalIPAddress(ipAddress), "GETIP");
#endif

#ifdef USE_HTP_TIME_SYNC
  // Remove any existing HTP servers
  while (true) {
    String servers;
    modem.getHTPServers(servers);
    servers.trim();
    if (servers.length() == 0) break;
    modem.delHTPServer(0);
  }

  // Set the HTP server and update time from HTP
  RETRY(modem.addHTPServer(SURVEILLANCE_API, 80), "HTPADD");
  RETRY(modem.setTimeFromHTP(), "HTPSET");
#endif // USE_HTP_TIME_SYNC

  // Set the GPS antenna power
#ifdef USE_ACTIVE_GPS_ANTENNA
  RETRY(modem.setGPSAntennaVoltage(ACTIVE_GPS_ANTENNA_MILLIVOLTS), "GPSVOLT");
  RETRY(modem.setGPSAntennaPower(true), "GPSPOW");
#else
  RETRY(modem.setGPSAntennaPower(false), "GPSPOW");
#endif

  // Start GPS
  if (!modem.getGPSAlive()) {
    // Not started so start
    modem.startGPS();
    RETRY(modem.getGPSAlive(), "GPS");
  }
  
  // Get common channel state
  byte network, session0, session1;
  RETRY(modem.getCommonChannelState(network, session0, session1), "CC");

  // Ensure the common channel is stopped
  if (network != 0) {
    modem.setCommonChannelClose(0);
    modem.setCommonChannelClose(1);
    RETRY(modem.stopCommonChannel(), "CCSTOP");
  }
  RETRY(modem.getCommonChannelState(network, session0, session1) && network == 0, "CCOFF");

  // Configure the common channel
  RETRY(modem.setCommonChannelMode(false), "CCMODE");
  RETRY(modem.setCommonChannelParam(false, true), "CCPARAM");

  // Start the common channel
  RETRY(modem.startCommonChannel(), "CCSTART");
  RETRY(modem.getCommonChannelState(network, session0, session1) && network == 4, "CCON");
  RETRY(modem.getCommonChannelState(network, session0, session1) && network == 4 && session0 == 1 && session1 == 1, "CCRDY");

  // GPS, network and common channel started so flash the power LED fast
  powerLed.setInterval(LED_FLASH_FAST);

  // Run loop
  unsigned long last = millis();
  byte session = 0;
  unsigned long pairStart = 0;
  bool paired = false;
  int failedAuth = 0;
  while (true) {
    // Only loop each interval
    unsigned long now = millis();
    if (now - last < POSITION_INTERVAL) {
      delay(POSITION_INTERVAL / 10);
      continue;
    }
    last = now;

    // Get the channel stream for the session we're currently using
    CaptureStream *channel = modem.getCommonChannelStream(session);
    Http http(channel);
  
    // Ensure SSL session is established correctly
    if (modem.getCommonChannelState(network, session0, session1)) {

      // Check if network is down and if it is reset by breaking out and restarting the main loop
      if (network != 4) {
        // Full device reset
        break;
      }

      // Get the state or the current and other sessions
      byte currentState = session == 0 ? session0 : session1;
      byte otherState = session == 0 ? session1 : session0;

      // Ensure the other session is closed and ready
      if (otherState != 1) {
        modem.setCommonChannelClose(session == 0 ? 1 : 0);
      }

      // Ensure the current session is connected
      if (currentState != 4) {
        powerLed.setInterval(LED_FLASH_FAST);
        modem.setCommonChannelOpen(session, SURVEILLANCE_API, 443, 2);
        continue;
      }

      // SSL established so turn on the power LED
      powerLed.setOn();
    }

    // If we're not paired see if the stored token is valid to use
    if (!paired) {
      // Read clock
      time_t now;
      if (modem.getClock(now)) {
        // Check if the currently stored token has enough time left on it without needing to pair
        time_t diff = SETTINGS.getTokenExpire() - now;
        if (diff > PAIR_EXPIRY_MIN_SEC && diff < PAIR_EXPIRY_MAX_SEC && SETTINGS.checkToken()) {
          paired = true;
          failedAuth = 0;
          pairLed.setInterval(LED_FLASH_FAST);
        }
      }
    }

    // Attempt to pair if the pair button is long pressed
    if (pairButton.isLongPressed()) {
      // Starting flashing the pair LED as we pair
      pairLed.setInterval(LED_FLASH_SLOW);
      pairStart = millis();
    }

    // If we're pairing attempt to get a token
    if (pairStart > 0) {
      // Check if we've timed out
      if (millis() - pairStart > PAIR_TIMEOUT) {
        pairStart = 0;
        pairLed.setOff();
        continue;
      }

      // Make request for token
      channel->flush();
      String uri(PAIR_URI_PREFIX);
      uri += imei;
      http.sendRequestLine(HTTP_METHOD_POST, uri);
      http.sendRequestHeader(HTTP_HEADER_HOST, SURVEILLANCE_API);
      http.sendRequestHeader(HTTP_HEADER_CONNECTION, HTTP_HEADER_CONNECTION_KEEP_ALVE);
      http.sendRequestHeader(HTTP_HEADER_USER_AGENT, SCOUT_USER_AGENT);
      http.sendRequestHeader(HTTP_HEADER_CONTENT_TYPE, MIME_CONTENT_TYPE_JSON);
      http.sendRequestHeader(HTTP_HEADER_CONTENT_LENGTH, String(0));
      http.sendRequestBody();
      http.sendRequestEnd();
      channel->flush();

      // Read HTTP status
      uint16_t status = 0;
      String description;
      if (!http.recvResponseLine(status, description)) continue;
      switch (status) {
        case HTTP_OK:
          break;
        //case HTTP_NOT_FOUND:
        default:
          channel->flush();
          delay(PAIR_INTERVAL);
          continue;
      }

      // Read till body
      String name, value;
      while (http.recvResponseHeader(name, value));
      if (!http.recvResponseBody()) continue;

      // Read till access token
      if (!channel->find(ACCESS_TOKEN_JSON, ACCESS_TOKEN_JSON_LEN)) continue;

      // Set access token in settings
      pairLed.setInterval(LED_FLASH_FAST);
      SETTINGS.setToken(channel);

      // Read till token expiry
      if (!channel->find(EXPIRES_AT_JSON, EXPIRES_AT_JSON_LEN)) continue;

      // Get expiry data
      String expiry;
      if (!channel->find('"')) continue;
      channel->startCapture(&expiry);
      if (!channel->find('"')) continue;
      channel->stopCapture();

      // Set expiry in settings
      SETTINGS.setTokenExpire(getTokenExpireTime(expiry));

      // Read to the end
      http.recvResponseEnd();

      // Set paired
      paired = true;
      pairStart = 0;
      failedAuth = 0;
    }

    // If paired get and send GPS location
    if (paired) {
      // Read clock
      tm clock;
      clock.tm_year = 0;
      long tz = 0;
      if (!modem.getClock(clock, tz)) continue;

      // Read GPS location
      double lat = 0, lng = 0, alt = 0, speed = 0, course = 0;
      if (!modem.getGPSInfo(lat, lng, alt, speed, course)) continue;

      // Check if we have at least a clock and GPSVOLT
      if (clock.tm_year == 0 || (lat == 0 && lng == 0 && alt == 0 && speed == 0 && course == 0)) {
        continue;
      }

      // Start building a position report
      PositionReport report;
      report.setSensorId(imei);
      report.setId(reportId++);
      report.setPosition(lat, lng);
      report.setTimestamp(clock);

      // Read voltage
      float voltage;
      if (modem.getVoltage(voltage)) {
        report.setVoltage(voltage);
      }

      // Read temperature
      int temperature;
      if (modem.getTemperature(temperature)) {
        report.setTemperature(temperature);
      }

      // Get signal quality
      byte rssi, ber;
      if (modem.getSignalQuality(rssi, ber)) {
        report.setRssi(rssi);
        report.setBer(ber);
      }

      // TODO: Convert speed and course to velocity vector

      // Send the HTTP POST header and position report
      channel->flush();
      http.sendRequestLine(HTTP_METHOD_POST, REPORT_URI);
      http.sendRequestHeader(HTTP_HEADER_HOST, SURVEILLANCE_API);
      http.sendRequestHeader(HTTP_HEADER_CONNECTION, HTTP_HEADER_CONNECTION_KEEP_ALVE);
      http.sendRequestHeader(HTTP_HEADER_USER_AGENT, SCOUT_USER_AGENT);
      http.sendRequestHeader(HTTP_HEADER_CONTENT_TYPE, MIME_CONTENT_TYPE_JSON);
      http.sendRequestHeader(HTTP_HEADER_CONTENT_LENGTH, String(report.length()));
      http.sendRequestHeader(HTTP_HEADER_AUTHORIZATION, AUTHORIZATION_BEARER, SETTINGS.getToken());
      http.sendRequestBody();
      report.writeJson(channel);
      channel->flush();

      // Wait for a response
      uint16_t status = 0;
      String description;
      if (!http.recvResponseLine(status, description)) continue;
      channel->flush();
      switch (status) {
        case HTTP_ACCEPTED:
          break;
        case HTTP_UNAUTHORIZED:
          failedAuth++;
          if (failedAuth > MAX_FAILED_AUTH) {
            paired = false;
            pairLed.setOff();
          }
          continue;
        default:
          continue;
      }

      // Flash the pair LED
      pairLed.setFlash(LED_FLASH_PING);
    }
  }
}

time_t getTokenExpireTime(String expire) {
  tm convert;
  convert.tm_year = expire.substring(0, 4).toInt() - 1900;
  convert.tm_mon = expire.substring(5, 7).toInt() - 1;
  convert.tm_mday = expire.substring(8, 10).toInt();
  convert.tm_hour = expire.substring(11, 13).toInt();
  convert.tm_min = expire.substring(14, 16).toInt();
  convert.tm_sec = expire.substring(17, 19).toInt();
  time_t time =  mktime(&convert);
  return time;
}
