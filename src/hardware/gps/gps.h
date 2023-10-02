#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// ======================= GPS HEADER=====================

class GPS
{
public:
    GPS(uint8_t pinRx, uint8_t pinTx);
    void setup();
    void displayGPSInfo();
    TinyGPSPlus module;
    SoftwareSerial port;


private:
    void sendCommand(const String command);
    bool _debugMode;
};
