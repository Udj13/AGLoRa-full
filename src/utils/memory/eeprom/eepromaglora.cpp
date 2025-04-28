/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#include <Arduino.h>
#include "../../../settings/settings.h"
#include "eepromaglora.h"
#include "../../crc/crc.h"

#include <EEPROM.h>

EEPROMAglora::EEPROMAglora()
{
    dataSize = sizeof(DATA);
    EEPROMStorageSize = (EEPROM.length() - EEPROM_BEGIN_ADDRESS) / EEPROMDataSize;
}

void EEPROMAglora::setup()
{
    EEPROMStorageIndex = 0;
    incrementCounter = 0;
    storageOverwrite = false;

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("📀[EEPROM storage: Start EEPROM initialization. Size of memory: "));
    Serial.print(EEPROM.length());
    Serial.println(F(" bytes]"));
    Serial.print(F("📀[EEPROM storage: "));
    Serial.print(EEPROMStorageSize);
    Serial.println(F(" track points can be saved.]"));
    Serial.println(F("\tCRC check symbols: ✅ CRC OK, ⛔️ CRC ERROR, ⬜ empty memory cell, 👉 - current cell"));
    Serial.println(F("\tFormat: {COUNTER}➜ {CRC} {CRC check symbol}, 255 - default EEPROM value "));
    Serial.print(F("\tFinding the index of the last record. Read memory... "));
#endif

    unsigned int prevIncCounter = 0;

    for (unsigned int i = 0; i < EEPROMStorageSize; i++)
    {
        EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, eepromdata);

#if DEBUG_MODE && DEBUG_MEMORY
        Serial.print(F("."));
        if ((i + 1) % 50 == 0)
            Serial.println();
#endif

        if (eepromdata.counter == 255)
        { // check empty EEPROM cell
            if (i == 0)
            {
                break;
            }
            EEPROMStorageIndex = i;
            incrementCounter = prevIncCounter + 1;
            if (incrementCounter >= 255)
            {
                incrementCounter = 0;
            }
            break;
        }

//        if (abs(eepromdata.counter - prevIncCounter) > 1)
        if ((eepromdata.counter > prevIncCounter ? 
            eepromdata.counter - prevIncCounter : 
            prevIncCounter - eepromdata.counter) > 1)
        { // not in sequence
            if (i != 0)
            {
                if (prevIncCounter != 254)
                { // exclude the option ...252,253,254,0,1,2...
                    EEPROMStorageIndex = i;
                    incrementCounter = prevIncCounter + 1;
                    if (incrementCounter >= 255)
                    {
                        incrementCounter = 0;
                    }
                    if (eepromdata.counter != 255)
                    {
                        storageOverwrite = true;
                    }
                    break;
                }
            }
        }
        prevIncCounter = eepromdata.counter;
    }

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println();
    Serial.print(F("\tNext record will be "));
    Serial.print(EEPROMStorageIndex + 1);
    Serial.print(F(",\tstorageOverwrite = "));
    Serial.println(storageOverwrite);
#endif
}

bool EEPROMAglora::checkUnique(DATA *newPoint)
{
    if (strcmp(newPoint->name, NAME) == 0)
    {
#if DEBUG_MODE && DEBUG_MEMORY
        Serial.println(F("📀[EEPROM storage: returned package 🔄 ]"));
#endif
        return false;
    }

    for (unsigned int i = 0; i < EEPROMStorageSize; i++)
    {
        DATA *eepromPoint = load(i);
        if ((newPoint->name == eepromPoint->name) &&
            (newPoint->year == eepromPoint->year) &&
            (newPoint->month == eepromPoint->month) &&
            (newPoint->day == eepromPoint->day) &&
            (newPoint->hour == eepromPoint->hour) &&
            (newPoint->minute == eepromPoint->minute) &&
            (newPoint->second == eepromPoint->second))
        {
#if DEBUG_MODE && DEBUG_MEMORY
            Serial.println(F("📀[EEPROM storage: data already exist‼️⛔️]"));
#endif
            return false;
        }
    }
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println(F("📀[EEPROM storage: new data checked ✅]"));
#endif
    return true;
}

/// @brief Save data to storage
/// @param newData that needs to be added to the storage
/// @return the index of added data
unsigned int EEPROMAglora::save(DATA *newData)
{
    eepromdata.counter = incrementCounter;
    eepromdata.data = *newData;
    eepromdata.crc = calculateCRC((unsigned char *)newData, sizeof(DATA));
    EEPROM.put(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, eepromdata);

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("📀[EEPROM storage: New data added. Address: "));
    Serial.print(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS);
    Serial.print(F(", index: "));
    Serial.print(EEPROMStorageIndex);
    Serial.print(F(", counter: "));
    Serial.print(eepromdata.counter);
    Serial.print(F(", CRC: "));
    Serial.print(eepromdata.crc);
    Serial.println(F("]"));
#endif

    EEPROMStorageIndex++;
    if (EEPROMStorageIndex >= EEPROMStorageSize)
    {
        EEPROMStorageIndex = 0;
    }

    incrementCounter++;
    if (incrementCounter >= 255)
    {
        incrementCounter = 0;
    }

    return incrementCounter;
}

DATA *EEPROMAglora::load(unsigned int index)
{
    EEPROM.get(index * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, eepromdata);
    return &eepromdata.data;
}

void EEPROMAglora::clearAllPositions()
{

#if DEBUG_MODE && DEBUG_MEMORY
    const byte rowLength = 120;
    int cellsCounter = 0;
    Serial.println(F("📀[EEPROM storage: clearing memory.]"));
    Serial.print(F("\t"));
#endif

    unsigned char memoryCell = 0;
    for (unsigned int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.get(i + EEPROM_BEGIN_ADDRESS, memoryCell);
        if (memoryCell != 255)
        { // 255 - default value in EEPROM
            memoryCell = 255;
            EEPROM.put(i + EEPROM_BEGIN_ADDRESS, memoryCell);
#if DEBUG_MODE && DEBUG_MEMORY
            Serial.print(F("#"));
            if (++cellsCounter % rowLength == 0)
            {
                Serial.println();
                Serial.print(F("\t"));
            }

#endif
        }
    }
    EEPROMStorageIndex = 0;
    incrementCounter = 0;
    storageOverwrite = false;

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println();
#endif
}

bool EEPROMAglora::checkCRC()
{
    const byte rowLength = 12; // how many characters will be printed in a row
    const byte rowDivider = 4; // split string for better view
    bool result = true;

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("📀[EEPROM storage: checking CRC, "));
    Serial.print(EEPROMStorageIndex);
    Serial.print(F("/"));
    Serial.print(EEPROMStorageSize);
    Serial.print(F(" cells are used, storageOverwrite is "));
    Serial.print(storageOverwrite);
    Serial.println(F("]"));
#endif

#if DEBUG_MODE && DEBUG_MEMORY // Memory visualisation
    Serial.print(F("\t "));
    for (unsigned int i = 0; i < rowLength; ++i)
    {
        Serial.print(F("🔻"));
        if (i < 10)
            Serial.print(F("0"));
        Serial.print(i + 1);
        Serial.print(F("⎻⎻⎻⎻⎻⎻⎻"));

        if ((i + 1) % rowDivider == 0)
            Serial.print(F("   "));
    }
    Serial.println();
    Serial.print(F("\t"));
#endif

    for (unsigned int i = 0; i < EEPROMStorageSize; i++)
    {
        EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, eepromdata);

        unsigned char crc = calculateCRC((unsigned char *)&eepromdata.data, sizeof(eepromdata.data));

#if DEBUG_MODE && DEBUG_MEMORY

        if (eepromdata.counter < 100)
            Serial.print(F(" "));
        if (eepromdata.counter < 10)
            Serial.print(F(" "));

        Serial.print(F(" "));
        Serial.print(eepromdata.counter);

        if (i == EEPROMStorageIndex - 1)
        {
            Serial.print(F("👉"));
        }
        else
        {
            Serial.print(F("➜ "));
        }

        if (eepromdata.crc < 100)
            Serial.print(F("0"));
        if (eepromdata.crc < 10)
            Serial.print(F("0"));

        Serial.print(eepromdata.crc);
#endif

        if (eepromdata.counter != 255)
        {
            if (crc == eepromdata.crc)

            {
#if DEBUG_MODE && DEBUG_MEMORY
                Serial.print(F("✅"));
#endif
            }
            else
            {
#if DEBUG_MODE && DEBUG_MEMORY
                Serial.print(F("⛔️"));
#endif
                result = false;
            }
        }
        else
        {
#if DEBUG_MODE && DEBUG_MEMORY
            Serial.print(F("⬜"));
#endif
        }

#if DEBUG_MODE && DEBUG_MEMORY // Memory visualisation
        if ((i + 1) % rowLength == 0)
        {
            Serial.println();
            Serial.println();
            Serial.print(F("\t"));
        }
        else
        {
            if ((i + 1) % rowDivider == 0)
                Serial.print(F(" · "));
        }
#endif
    }

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println();
#endif

    return result;
}

bool EEPROMAglora::checkCRC(unsigned int index)
{
    EEPROM.get(index * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, eepromdata);
    const byte crc = calculateCRC((unsigned char *)&eepromdata.data, dataSize);
    if (eepromdata.crc == crc)
        return true;
    return false;
}

unsigned int EEPROMAglora::getSize()
{
    return EEPROMStorageSize;
}

unsigned int EEPROMAglora::getIndex()
{
    return EEPROMStorageIndex;
}

bool EEPROMAglora::getStorageOverwrite()
{
    return storageOverwrite;
}
