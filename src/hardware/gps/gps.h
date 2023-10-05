#ifndef GPS_INCLUDED
#define GPS_INCLUDED


#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// ======================= GPS HEADER=====================

class GPS
{
    SoftwareSerial port;
    TinyGPSPlus tinygps;
public:
    GPS(uint8_t pinRx, uint8_t pinTx, long speed);
    void setup();
    void listen();
    void printGPSInfo();
    TinyGPSPlus module;


private:
    bool _debugMode;

};

#endif