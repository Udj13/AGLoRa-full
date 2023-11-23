#include <Arduino.h>
#include "../../../settings/settings.h"

#include "sram.h"
#include "../../crc/crc.h"

SRAM::SRAM()
{
    dataSize = sizeof(DATA);
}

void SRAM::setup()
{
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("💾[SRAM storage: memory is ready. SRAM_STORAGE_SIZE="));
    Serial.print(SRAM_STORAGE_SIZE + 1);
    Serial.print(F(" ("));
    Serial.print((SRAM_STORAGE_SIZE + 1) * sizeof(DATA));
    Serial.print(F(" bytes)"));
    Serial.println(F("]"));
    Serial.println(F("\t CRC check symbols: ✅ CRC OK, ⛔️ CRC ERROR, ⬜ empty memory cell, underlined CRC\u0332 - current cell"));
#endif
}

/// @brief Checking new data for uniqueness
/// @param loraDataPacket
/// @return true if the same data is not exist
bool SRAM::checkUnique(DATA *newPoint)
{
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("💾[SRAM storage: checking data for uniqueness 📦 ↔ 📦 ↔ 📦. First check, is <"));
    Serial.print(newPoint->name);
    Serial.print(F("> equal to <"));
    Serial.print(NAME);
    Serial.println(F(">]"));
#endif

    if (strcmp(newPoint->name, NAME) == 0)
    {
#if DEBUG_MODE && DEBUG_MEMORY
        Serial.print(F("💾[SRAM storage: returned package 🔄, because "));
        Serial.print(newPoint->name);
        Serial.println(F(" it's me!]"));
#endif
        return false;
    }

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println(F("💾[SRAM storage: checking data for uniqueness 📦 ↔ 📦 ↔ 📦. Second check, find the coordinates and time duplication "));
    Serial.print(F("\t"));
#endif

    const unsigned int maxIndex = storageOverwrite ? SRAM_STORAGE_SIZE : storageIndex;
    for (unsigned int i = 0; i < maxIndex; ++i)
    {
#if DEBUG_MODE && DEBUG_MEMORY
        Serial.print(F("#"));

        Serial.println();
        Serial.print(newPoint->name); Serial.print(F("\t - "));Serial.println(storage[i].data.name);
        Serial.print(newPoint->year); Serial.print(F("\t - "));Serial.println(storage[i].data.year);
        Serial.print(newPoint->month); Serial.print(F("\t - "));Serial.println(storage[i].data.month);
        Serial.print(newPoint->day); Serial.print(F("\t - "));Serial.println(storage[i].data.day);
        Serial.print(newPoint->hour); Serial.print(F("\t - "));Serial.println(storage[i].data.hour);
        Serial.print(newPoint->minute); Serial.print(F("\t - "));Serial.println(storage[i].data.minute);
        Serial.print(newPoint->second); Serial.print(F("\t - "));Serial.println(storage[i].data.second);
#endif

        if ((strcmp(newPoint->name, storage[i].data.name) == 0) &&
            (newPoint->year == storage[i].data.year) &&
            (newPoint->month == storage[i].data.month) &&
            (newPoint->day == storage[i].data.day) &&
            (newPoint->hour == storage[i].data.hour) &&
            (newPoint->minute == storage[i].data.minute) &&
            (newPoint->second == storage[i].data.second))
        {
#if DEBUG_MODE && DEBUG_MEMORY
            Serial.println();
            Serial.println(F("💾[SRAM storage: data already exist‼️⛔️]"));
#endif
            return false;
        }
    }
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println();
    Serial.println(F("💾[SRAM storage: new data ✅]"));
#endif
    return true;
}

/// @brief Save data to storage
/// @param newData data that needs to be added to the storage
/// @return the index of added data
unsigned int SRAM::save(DATA *newData)
{
    storage[storageIndex].data = *newData;
    storage[storageIndex].crc = calculateCRC((unsigned char *)newData, dataSize);

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("💾[SRAM storage saving: data from "));
    Serial.print(storage[storageIndex].data.name);
    Serial.print(F(" was added. Memory: "));
    Serial.print(storageIndex + 1);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE + 1);
    Serial.print(F(", CRC "));
    Serial.print(storage[storageIndex].crc);
    Serial.println(F(" ✅]"));
#endif

    unsigned int addedIindex = storageIndex;
    storageIndex++;
    if (storageIndex >= SRAM_STORAGE_SIZE)
    {
        storageIndex = 0;
        storageOverwrite = true;
    }

    return addedIindex;
}

/// @brief Loading data from memory to loraDataPacket by index
/// @param loraDataPacket pointer
/// @param index index of data in memory
/// @return true if success
DATA *SRAM::load(unsigned int index)
{
    return &storage[index].data;
}

void SRAM::clearAllPositions()
{
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println(F("💾[SRAM storage: clearing memory 🫙]"));
#endif
    storageIndex = 0;
    storageOverwrite = false;
}

bool SRAM::checkCRC()
{
#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("💾[SRAM storage: checking CRC, "));
    Serial.print(storageIndex);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.print(F(" cells are used, storageOverwrite is "));
    Serial.print(storageOverwrite);
    Serial.println(F("]"));
#endif

    const byte rowLength = 12; // how many characters will be printed in a row
    const byte rowDivider = 4; // split string for better view
    bool result = true;
    byte crc = 0;

    if ((storageIndex == 0) && (!storageOverwrite))
    {
#if DEBUG_MODE && DEBUG_MEMORY
        Serial.println(F("💾[SRAM storage: memory is empty]"));
#endif
        return result;
    }

    const unsigned int maxIndex = storageOverwrite ? (SRAM_STORAGE_SIZE - 1) : (storageIndex - 1);

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("\t"));
#endif

    for (unsigned int i = 0; i < SRAM_STORAGE_SIZE; ++i)
    {
        if (i <= maxIndex)
        {
            crc = calculateCRC((unsigned char *)&storage[i], dataSize);
            if (storage[i].crc == crc)
            {
#if DEBUG_MODE && DEBUG_MEMORY
                Serial.print(F(" ✅"));
#endif
            }
            else
            {
#if DEBUG_MODE && DEBUG_MEMORY
                Serial.print(F(" ⛔️"));
#endif
                result = false;
            }
#if DEBUG_MODE && DEBUG_MEMORY
            if (crc < 100)
                Serial.print(F("0"));
            if (crc < 10)
                Serial.print(F("0"));
            Serial.print(crc);
            if ((i == storageIndex - 1) ||
                ((i == 0) && (storageOverwrite)))
            {
                Serial.print(F("\u0332")); // underline active memory cell
            }
#endif
        }
        else
        {
#if DEBUG_MODE && DEBUG_MEMORY
            Serial.print(F(" ⬜"));
            Serial.print(F("   "));
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

bool SRAM::checkCRC(SRAMDATA *point)
{
    const byte crc = calculateCRC((unsigned char *)point, dataSize);
    if (point->crc == crc)
        return true;
    return false;
}

bool SRAM::checkCRC(unsigned int index)
{
    const byte crc = calculateCRC((unsigned char *)&storage[index], dataSize);
    if (storage[index].crc == crc)
        return true;
    return false;
}

unsigned int SRAM::getSize()
{
    return SRAM_STORAGE_SIZE;
}

unsigned int SRAM::getIndex()
{
    return storageIndex;
}

bool SRAM::getStorageOverwrite()
{
    return storageOverwrite;
}
