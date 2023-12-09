// Project AGLoRa: https://github.com/Udj13/AGLoRa/
// Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
// License: http://opensource.org/licenses/MIT

#ifndef SRAM_INCLUDED
#define SRAM_INCLUDED

#include "../imemory.h"
#include "sramdata.h"

class SRAM: public IMemory
{
public:
    SRAM();
    void setup();
    bool checkUnique(DATA *newPoint);
    unsigned int save(DATA *newData); //function returns the index
    DATA * load(unsigned int index);
    void clearAllPositions();
    bool checkCRC();
    bool checkCRC(unsigned int index);
    unsigned int getSize();
    unsigned int getIndex();
    bool getStorageOverwrite();

private:
    SRAMDATA storage[SRAM_STORAGE_SIZE];
    unsigned int storageIndex = 0;
    bool storageOverwrite = false;
    byte dataSize;
    bool checkCRC(SRAMDATA *point);
};

#endif




