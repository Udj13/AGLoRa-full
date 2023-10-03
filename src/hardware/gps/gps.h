#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// ======================= GPS HEADER=====================

class GPS
{
public:
    GPS(uint8_t pinRx, uint8_t pinTx, long speed);
    void setup();
    void displayGPSInfo();
    TinyGPSPlus module;
    SoftwareSerial port;


private:
    bool _debugMode;

};
