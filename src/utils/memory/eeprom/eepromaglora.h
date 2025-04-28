/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#ifndef EEPROMAGLORA_INCLUDED
#define EEPROMAGLORA_INCLUDED

#include "../imemory.h"
#include "eepromdata.h"

class EEPROMAglora : public IMemory
{
public:
    EEPROMAglora();
    void setup();
    bool checkUnique(DATA *newPoint);
    unsigned int save(DATA *newData);
    DATA *load(unsigned int index);
    void clearAllPositions();
    bool checkCRC();
    bool checkCRC(unsigned int index);
    unsigned int getSize();
    unsigned int getIndex();
    bool getStorageOverwrite();

private:

    EEPROMDATA eepromdata;
    unsigned int EEPROMStorageIndex = 0;  // index in memory (address = EEPROMStorageIndex * EEPROMDataSize)  
    unsigned int incrementCounter = 0;             // min 0, max 254 (because default EEPROM is 255)

    unsigned int EEPROMStorageSize;
    byte dataSize;
    bool storageOverwrite = false;

    const unsigned char EEPROMDataSize = sizeof(EEPROMDATA);
};

#endif
