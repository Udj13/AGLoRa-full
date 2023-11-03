#include "sram.h"
#include "../../crc.h"

void SRAM::setup()
{
#if DEBUG_MODE
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
bool SRAM::checkUnique(DATA *loraDataPacket)
{
    if (loraDataPacket->name == NAME)
    {
#if DEBUG_MODE
        Serial.println(F("💾[SRAM storage: returned package 🔄 ]"));
#endif
        return false;
    }

    const unsigned int maxIndex = storageOverflow ? SRAM_STORAGE_SIZE : storageIndex;
    for (unsigned int i = 0; i < maxIndex; ++i)
    {
        if ((loraDataPacket->name == storage[i].name) &&
            (loraDataPacket->year == storage[i].year) &&
            (loraDataPacket->month == storage[i].month) &&
            (loraDataPacket->day == storage[i].day) &&
            (loraDataPacket->hour == storage[i].hour) &&
            (loraDataPacket->minute == storage[i].minute) &&
            (loraDataPacket->second == storage[i].second))
        {
#if DEBUG_MODE
            Serial.println(F("💾[SRAM storage: data already exist‼️⛔️]"));
#endif
            return false;
        }
    }
#if DEBUG_MODE
    Serial.println(F("💾[SRAM storage: new data checked ✅]"));
#endif
    return true;
}

void SRAM::save(DATA *newData)
{
    storage[storageIndex] = *newData;
    byte size = sizeof(storage[storageIndex]) - sizeof(storage[storageIndex].ttlOrCrc);
    storage[storageIndex].ttlOrCrc = calculateCRC((unsigned char *)newData, size);

#if DEBUG_MODE
    Serial.print(F("💾[SRAM storage: New data from "));
    Serial.print(storage[storageIndex].name);
    Serial.print(F(" (TTL="));
    Serial.print(storage[storageIndex].ttlOrCrc);
    Serial.print(F(") was added. Memory: "));
    Serial.print(storageIndex + 1);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE + 1);
    Serial.print(F(", CRC "));
    Serial.print(storage[storageIndex].ttlOrCrc);
    Serial.println(F(" ✅]"));
#endif

    storageIndex++;
    if (storageIndex >= SRAM_STORAGE_SIZE)
    {
        storageIndex = 0;
        storageOverflow = true;
    }
}

void SRAM::clearAllPositions()
{
#if DEBUG_MODE
    Serial.println(F("💾[SRAM storage: clearing memory 🫙]"));
#endif
    storageIndex = 0;
    storageOverflow = false;
}

bool SRAM::checkCRC()
{
#if DEBUG_MODE
    Serial.print(F("💾[SRAM storage: checking CRC, "));
    Serial.print(storageIndex);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.print(F(" cells are used, storageOverflow is "));
    Serial.print(storageOverflow);
    Serial.println(F("]"));
#endif

    const byte rowLength = 15; // how many characters will be printed in a row
    const byte rowDivider = 5; // split string for better view
    bool result = true;
    byte crc = 0;
    byte size = sizeof(storage[0]) - sizeof(storage[0].ttlOrCrc);

    if ((storageIndex == 0) && (!storageOverflow))
    {
#if DEBUG_MODE
        Serial.println(F("💾[SRAM storage: memory is empty]"));
#endif
        return result;
    }

    const unsigned int maxIndex = storageOverflow ? (SRAM_STORAGE_SIZE - 1) : (storageIndex - 1);

#if DEBUG_MODE
    Serial.print(F("\t"));
#endif

    for (unsigned int i = 0; i < SRAM_STORAGE_SIZE; ++i)
    {
        if (i <= maxIndex)
        {
            crc = calculateCRC((unsigned char *)&storage[i], size);
            if (storage[i].ttlOrCrc == crc)
            {
#if DEBUG_MODE
                Serial.print(F(" ✅"));
#endif
            }
            else
            {
#if DEBUG_MODE
                Serial.print(F(" ⛔️"));
#endif
                result = false;
            }
#if DEBUG_MODE
            if (crc < 100)
                Serial.print(F("0"));
            if (crc < 10)
                Serial.print(F("0"));
            Serial.print(crc);
            if ((i == storageIndex - 1) || (i == SRAM_STORAGE_SIZE - 1))
            {
                Serial.print(F("\u0332")); // underline active memory cell
            }
#endif
        }
        else
        {
#if DEBUG_MODE
            Serial.print(F(" ⬜"));
            Serial.print(F("   "));
#endif
        }

#if DEBUG_MODE // Memory visualisation

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

#if DEBUG_MODE
    Serial.println();
#endif

    return result;
}
