#ifndef GPS_INCLUDED
#define GPS_INCLUDED


#include <Arduino.h>
#include <SoftwareSerial.h>

// install from Arduino IDE or PlatformIO
// Docs: http://arduiniana.org/libraries/tinygpsplus/
#include <TinyGPSPlus.h>

#include "../../settings/settings.h"
#include "../indication/indication.h"

// ======================= GPS HEADER=====================

class GPS
{
    SoftwareSerial gpsPort;
    TinyGPSPlus gpsModule;
public:
    GPS(uint8_t pinRx, uint8_t pinTx, long speed, INDICATION * indication);
    void setup();
    void updateLocation(DATA *dataPackage);


private:
    bool _debugMode;
    INDICATION * _indication;
    void printGPSInfo();
    void printReadingIndication(unsigned long start, unsigned int delay);
    char _readingIndicator = 0;
};

#endif