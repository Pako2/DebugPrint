/*
  ESP8266 Debug Print library
  Version 1.0
  Copyright (C) 2019 Lubos Ruckl

  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org/>
  =====================================================================

  Example sketch:
// ***********************************************************************************
// Used NTPclient library: https://github.com/arduino-libraries/NTPClient
// ----------------------------------------------------------------------

//#define SOFT_DEBUG   //When we want to use SoftwareSerial instead of HardwareSerial
#define SW_GPIO_TX 12

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include "DebugPrint.h"

const char *ssid       = "<SSID>";
const char *password   = "<PASSWORD>";
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

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    my_debug.print ( "." );
  }
  my_debug.println("\r\nWifi connected !");
  my_debug.print("IP address: ");
  my_debug.println(WiFi.localIP());

  timeClient.begin();
  timeClient.update();
  time_t _now = now();
  while (year(_now) == 1970) {
    _now = timeClient.getEpochTime();
    delay ( 500 );
    my_debug.print ( "." );
  }
  setTime(timeClient.getEpochTime());
  my_debug.println("\r\nGot NTP time !");
}

void loop() {
  timeClient.update();
  my_debug.print(F("Free Heap: "));
  my_debug.println(ESP.getFreeHeap());
  delay(30000);
}
// ***********************************************************************************
*/

#ifndef DebugPrint_h
#define DebugPrint_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include "SoftwareSerial.h"

class DebugPrint : public Stream
{
  private:
    bool _sw;
    uint16_t _port = 0;
    long _speed = 0;
    bool _tmstmp = false;
    int16_t _tzcorr = 0;
    static const uint8_t tssize = 22;
    static const uint8_t buffsize = 200 + tssize;
    char buff[buffsize + 3];
    uint8_t count = tssize;
    bool cl = false;
    void setTimestamp();
    Stream *_dev;
    WiFiServer _telser = WiFiServer(23);
    WiFiClient _telcli;
  public:
    DebugPrint(Stream *dev, bool sw);
    DebugPrint(Stream &dev, bool sw);
    virtual ~DebugPrint();
    virtual int read();
    virtual int available();
    virtual int peek();
    virtual void flush();
    void begin(long speed, uint16_t port = 0, bool tmstmp = false, int16_t tzcorr = 0);
    inline bool check_client();
    size_t write( uint8_t b);
    using Stream::write;
};
#endif
