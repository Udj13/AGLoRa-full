/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#ifndef GPS_INCLUDED
#define GPS_INCLUDED


#include <Arduino.h>
#include <SoftwareSerial.h>

#include "../../settings/settings.h"
#include "../indication/indication.h"

// ======================= GPS SECTION =====================
/*
   ____   ____    ____  
  / ___| |  _ \  / ___| 
 | |  _  | |_) | \___ \ 
 | |_| | |  __/   ___) |
  \____| |_|     |____/ 
                        
*/

// install from Arduino IDE or PlatformIO
// Docs: http://arduiniana.org/libraries/tinygpsplus/
//#include <TinyGPSPlus.h>

class GPS
{
    #if USE_HARDWARE_GPS_UART   
        HardwareSerial gpsPort;
    #else
        SoftwareSerial gpsPort;
    #endif
public:
    GPS(uint8_t pinRx, uint8_t pinTx, uint32_t speed, INDICATION * indication);
    void setup();
    void updateLocation(DATA *dataPackage);


private:
    bool _debugMode;
    INDICATION * _indication;
    void printReadingIndication(unsigned long start, unsigned int delay);
    char _readingIndicator = 0;
    bool processRMCSentence(String sentence, DATA *dataPackage);
    bool processGGASentence(String sentence, DATA *dataPackage);
    float convertDegMinToDecDeg(float degMin);
};

#endif