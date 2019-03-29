#include "DebugPrint.h"

#define ESTABLISHED 4

DebugPrint::DebugPrint(Stream *dev, bool sw) : _dev(dev)  {
  _sw = sw;
}
DebugPrint::DebugPrint(Stream &dev, bool sw) : _dev(&dev) {
  _sw = sw;
}
DebugPrint::~DebugPrint() {}

int DebugPrint::read() {
  return -1;
}
int DebugPrint::available() {
  return 0;
}
int DebugPrint::peek() {
  return -1;
}
void DebugPrint::flush() {
}

void DebugPrint::begin(long speed, uint16_t port, bool tmstmp, int16_t tzcorr) {
  _port = port;
  _speed = speed;
  _tmstmp = tmstmp;
  _tzcorr = tzcorr;
  if (_speed > 0) {
    _dev->flush();
    if (_sw) {
      static_cast<SoftwareSerial*>(_dev)->begin(speed);
    }
    else     {
      static_cast<HardwareSerial*>(_dev)->begin(speed);
    }
  }
  if (port > 0) {
    telbuff = malloc(TELNET_BUFF);
    _telser.begin(port);
    _telser.setNoDelay(true);
  }
}

inline bool DebugPrint::check_client()
{
  WiFiClient newClient = _telser.available();
  if ( !newClient ) {
    if ( ! _telcli ) {
      return false;
    }
    else if (_telcli.status() != ESTABLISHED) {
      _telcli.stop();
      return false;
    }
  }
  else
  {
    _telcli.stop();
    _telcli = newClient;
    _telcli.println(F("Welcome !\r\nESP8266 Debug Print 1.1"));
    _telcli.println(F("Only one client can be connected at the same time !\r\n"));
    // if (_speed > 0) {
    //   _dev->println("\nNew client connected !");
    // }
  }
  return _telcli.status() == ESTABLISHED;
}

void DebugPrint::setTimestamp() {
  time_t local = now() + _tzcorr;
  sprintf (buff, "%4d-%02d-%02d %02d:%02d:%02d  ", year(local), month(local), day(local), hour(local), minute(local), second(local));
  buff[tssize - 1] = 32; //replace null-terminator !!!
}

size_t DebugPrint::write( uint8_t b)
{
  buff[count++] = b;
  if (count >= buffsize) {
    setTimestamp();
    buff[buffsize] = 13;
    buff[buffsize + 1] = 10;
    if (_port > 0) {
      cl = check_client();
      if (cl) {
        if (telcnt > 0) {
          _telcli.write((char*)telbuff, telcnt);
          if (telcnt == TELNET_BUFF) {
            const char *warn = WARNING;
            _telcli.write(warn, 146);
          }
          telcnt = 0;
        }
        if (_tmstmp) {
          _telcli.write(buff, buffsize + 2);
        }
        else {
          _telcli.write(buff + tssize, buffsize - tssize + 2);
        }
      }
      else {//write to telnet buff
        uint8_t cnt = buffsize+2;
        size_t num = (cnt < (TELNET_BUFF - telcnt)) ? cnt : TELNET_BUFF - telcnt;
        memcpy((char*)telbuff + telcnt, buff, num);
        telcnt += num;
      }
    }
    if (_speed > 0) {
      buff[buffsize + 2] = 0;
      if (_tmstmp) {
        _dev->print(buff);
      }
      else {
        _dev->print(buff + tssize);
      }
    }
    count = tssize;
  }
  else {
    if (b == 10) {
      setTimestamp();
      if (_port > 0) {
        cl = check_client();
        if (cl) {
          if (telcnt > 0) {
            _telcli.write((char*)telbuff, telcnt);
            if (telcnt == TELNET_BUFF) {
              const char *warn = WARNING;
              _telcli.write(warn, 146);
            }
            telcnt = 0;
          }
          if (_tmstmp) {
            _telcli.write(buff, count);
          }
          else {
            _telcli.write(buff + tssize, count - tssize);
          }
        }
        else {//write to telnet buff
          size_t num = (count < (TELNET_BUFF - telcnt)) ? count : TELNET_BUFF - telcnt;
          memcpy((char*)telbuff + telcnt, buff, num);
          telcnt += num;
        }
      }
      if (_speed > 0)
      {
        buff[count] = 0;
        if (_tmstmp) {
          _dev->print(buff);
        }
        else {
          _dev->print(buff + tssize);
        }
      }
      count = tssize;
    }
  }
  return (size_t)1;
}

