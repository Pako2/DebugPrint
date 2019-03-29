// Used NTPclient library: https://github.com/arduino-libraries/NTPClient
//-----------------------------------------------------------------------

//#define SOFT_DEBUG   //When we want to use SoftwareSerial instead of HardwareSerial
#define SW_GPIO_TX 12

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include "DebugPrint.h"

const char *ssid       = "Abura";
const char *password   = "520516520516";
const char *serverPool = "pool.ntp.org";
int16_t tzoffset       = 3600;
unsigned long baud     = 115200;  // If we set baud = 0, it will disable Serial output
uint16_t port          = 23;      // If we set port = 0, it will disable telnet server
bool insertTimestamp   = true;    // We can enable/disable time stamping
int16_t tzcorr         = 0;       // Number of seconds to correct the timestamp.
// This can be equal to tzoffset when now() returns UTC time,
// 0 when now() returns local time

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, serverPool, tzoffset);

#ifdef SOFT_DEBUG
SoftwareSerial sSerial(-1, SW_GPIO_TX, false, 256);
DebugPrint my_debug(sSerial, true);
#else
DebugPrint my_debug(&Serial, false);
#endif
//#define my_debug Serial    // We can easily switch to regular Serial !

void setup() {
  my_debug.begin(baud, port, insertTimestamp, tzcorr);
  //my_debug.begin(115200);  // We can easily switch to regular Serial !
  my_debug.println();
  my_debug.println(F("\r\nWaiting for WiFi ..."));

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    my_debug.print(F("."));
  }
  my_debug.println(F("\r\nWiFi connected !"));
  my_debug.print(F("IP address: "));
  my_debug.println(WiFi.localIP());
  my_debug.println(F("Waiting for NTP time ..."));
  timeClient.begin();
  timeClient.update();
  time_t _now = now();
  while (year(_now) == 1970) {
    _now = timeClient.getEpochTime();
    delay(500);
    my_debug.print(F("."));
  }
  setTime(timeClient.getEpochTime());
  my_debug.println(F("\r\nGot NTP time !"));
}

void loop() {
  timeClient.update();
  my_debug.print(F("Free Heap: "));
  my_debug.println(ESP.getFreeHeap());
  delay(30000);
}
