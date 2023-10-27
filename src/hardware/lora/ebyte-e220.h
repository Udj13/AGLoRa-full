#ifndef LORA_INCLUDED
#define LORA_INCLUDED

#include <Arduino.h>
#include <SoftwareSerial.h>

// install from Arduino IDE or PlatformIO
// Docs: https://github.com/xreef/EByte_LoRa_E220_Series_Library
#include "LoRa_E220.h"


#include "../../settings/settings.h"


// ======================= LORA HEADER =====================

class LORA
{
    SoftwareSerial loraPort;
    LoRa_E220 e220ttl;
public:
    LORA(uint8_t pinRx, uint8_t pinTx, long speed, uint8_t aux, uint8_t m0, uint8_t m1, uint8_t ledPin);
    void setup();
    void send(DATA *loraDataPacket);
    bool hasNewData(DATA *loraDataPacket);


private:
    bool _debugMode;
    uint8_t _ledPin;
    void turnIndicatorOn();
    void turnIndicatorOff();
    ResponseStructContainer rsc;
};



#endif