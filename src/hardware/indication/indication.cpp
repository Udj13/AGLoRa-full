#include "indication.h"

INDICATION::INDICATION(uint8_t gpsLedPin, uint8_t loraLedPin, uint8_t bleLedPin, uint8_t memoryLedPin)
{
    _gpsLedPin = gpsLedPin;
    _loraLedPin = loraLedPin;
    _bleLedPin = bleLedPin;
    _memoryLedPin = memoryLedPin;

    pinMode(_gpsLedPin, OUTPUT);  // GPS indicator
    pinMode(_loraLedPin, OUTPUT); // LORA indicator
}

void INDICATION::gps(GPSStatuses status)
{
    switch (status)
    {
    case GPSStatuses::correct:
        digitalWrite(_gpsLedPin, HIGH);
        break;
    case GPSStatuses::invalid:
        digitalWrite(_gpsLedPin, LOW);
        break;
    case GPSStatuses::connectionError:
        digitalWrite(_gpsLedPin, LOW);
        break;
    default:
        digitalWrite(_gpsLedPin, LOW);
    }
}

void INDICATION::lora(LoRaStatuses status)
{
    switch (status)
    {
    case LoRaStatuses::dataReceived:
        digitalWrite(_loraLedPin, HIGH);
        loraLedStatus = true;
        break;
    case LoRaStatuses::dataTransmitted:
        digitalWrite(_loraLedPin, HIGH);
        loraLedStatus = true;
        break;
    case LoRaStatuses::error:
        digitalWrite(_loraLedPin, LOW);
        loraLedStatus = false;
        break;
    default:
        digitalWrite(_loraLedPin, LOW);
        loraLedStatus = false;
    }
    lastLoraUpdateTime = millis();
}

void INDICATION::ble(BLEStatuses status)
{
}

void INDICATION::memory(MemoryStatuses status)
{
}

void INDICATION::loop()
{
    if (loraLedStatus)
    {
        if ((lastLoraUpdateTime + (loraDelaySec * 1000)) < millis())
        {
            digitalWrite(_loraLedPin, LOW);

            loraLedStatus = false;
        }
    }
}
