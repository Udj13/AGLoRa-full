/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#ifndef INDICATION_INCLUDED
#define INDICATION_INCLUDED

#include <Arduino.h>

// ====================== INDICATION SECTION =======================
/*
  ___   _   _   ____    ___    ____      _      _____   ___    ___    _   _ 
 |_ _| | \ | | |  _ \  |_ _|  / ___|    / \    |_   _| |_ _|  / _ \  | \ | |
  | |  |  \| | | | | |  | |  | |       / _ \     | |    | |  | | | | |  \| |
  | |  | |\  | | |_| |  | |  | |___   / ___ \    | |    | |  | |_| | | |\  |
 |___| |_| \_| |____/  |___|  \____| /_/   \_\   |_|   |___|  \___/  |_| \_|
                                                                            
*/

enum class GPSStatuses
{
    correct,
    invalid,
    connectionError
};

enum class LoRaStatuses
{
    dataReceived,
    dataTransmitted,
    error
};

enum class BLEStatuses
{
    output,
    input,
    error
};

enum class MemoryStatuses
{
    read,
    write,
    crcError
};

class INDICATION
{
public:
    INDICATION(uint8_t gpsLedPin,
               uint8_t loraLedPin,
               uint8_t bleLedPin,
               uint8_t memoryLedPin);
    void gps(GPSStatuses status);
    void lora(LoRaStatuses status);
    void ble(BLEStatuses status);
    void memory(MemoryStatuses status);
    void loop();

private:
    uint8_t _gpsLedPin;
    uint8_t _loraLedPin;
    uint8_t _bleLedPin;
    uint8_t _memoryLedPin;

    bool loraLedStatus = false;
    const byte loraDelaySec = 1;
    unsigned long lastLoraUpdateTime;
};

#endif