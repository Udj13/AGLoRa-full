#ifndef EEPROM_INCLUDED
#define EEPROM_INCLUDED

#include <Arduino.h>
#include "eepromdata.h"
#include "../imemory.h"
#include "../../../settings/settings.h"

class EEPROMAglora : public IMemory
{
public:
    EEPROMAglora();
    void setup();
    bool checkUnique(DATA *loraDataPacket);
    unsigned int save(DATA *newData);
    DATA *load(unsigned int index);
    void clearAllPositions();
    bool checkCRC();
    bool checkCRC(DATA *loraDataPacket);
    bool checkCRC(unsigned int index);
    unsigned int getSize();
    unsigned int getIndex();
    bool getStorageOverwrite();

private:
    // DATA storage[SRAM_STORAGE_SIZE];
    // unsigned int storageIndex = 0;
    // bool storageOverwrite = false;
    // byte dataSizeWithoutCRC;

    EEPROMDATA EEPROMdata;

    unsigned int EEPROMStorageIndex = 0;  // index in memory (address = EEPROMStorageIndex * EEPROMDataSize)  
    unsigned int incrementCounter = 0;             // min 0, max 254 (because default EEPROM is 255)

    const unsigned char EEPROMDataSize = sizeof(EEPROMDATA);
    unsigned int EEPROMStorageSize = 0;
};

#endif
