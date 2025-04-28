/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

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
