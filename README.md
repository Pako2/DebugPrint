# DebugPrint - Library for ESP8266 debug printings
## Preface
I know that there are similar libraries for "print to telnet". 
For example: [RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug), [TelnetStream](https://github.com/jandrassy/TelnetStream), or [TelnetSpy](https://github.com/yasheena/telnetspy).
For various reasons, none of these projects suited me.
This is why this library was created.

## Initial requirements
At the beginning I had simple requirements:
1) Completely benevolent license.
2) Easy integration into an existing project.
3) Possibility to insert timestamp.
4) Option to use SoftwareSerial instead of HardwareSerial.
5) Possibility to enable and disable both Serial and Telnet independently.


## Integration into an existing project
It is easy. Two steps are enough to do this:
1) Creating a DebugPrint instance: `DebugPrint my_debug (&Serial, false);`
2) In the source text, replace all occurences (**except the above command**) of the word "Serial" with "my_debug".


## Output to software serial port
For output to a software serial port is used [SoftwareSerial](https://github.com/plerup/espsoftwareserial)  library.
```
#define SOFT_DEBUG  
#define SW_GPIO_TX 12  
#include <SoftwareSerial.h>
```


## Selection of hardware or software for serial port
```
#ifdef SOFT_DEBUG
SoftwareSerial swSerial(-1, SW_GPIO_TX, false, 256);
DebugPrint my_debug(swSerial, true);
#else
DebugPrint my_debug(&Serial, false);
#endif
```


## "Begin" options
To make the prints work, you need to use the "begin" command:
`my_debug.begin(baud, port, insertTimestamp, tzcorr);`  
**baud**            = serial port speed. Max. SoftwareSerial's speed is 115200. If you select 0, no output will go to the serial port.  
**port**            = port used for telnet (usually 23). If you select 0, the telnet server will not be used.  
**insertTimestamp** = true or false. Use this argument to enable or disable the timestamp insertion option.  
**tzcorr**          = number of seconds to correct time (according to time zone). If the **now()** command returns the correct local time, tzcorr will be 0. Otherwise, it will usually be a time zone offset.