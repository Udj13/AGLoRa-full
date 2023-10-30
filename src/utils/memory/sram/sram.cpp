#include "sram.h"
#include "../../crc.h"

void SRAM::setup()
{
#if DEBUG_MODE
    Serial.print(F("💾[SRAM storage: memory is ready. SRAM_STORAGE_SIZE="));
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.print(F(" ("));
    Serial.print(SRAM_STORAGE_SIZE * sizeof(DATA));
    Serial.print(F(" bytes)"));
    Serial.println(F("]"));
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
    for (unsigned int i = 0; i <= maxIndex; ++i)
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
    storageIndex++;
    if (storageIndex >= SRAM_STORAGE_SIZE)
    {
        storageIndex = 0;
        storageOverflow = true;
    }
    storage[storageIndex] = *newData;
    storage[storageIndex].ttlOrCrc = calculateCRC((unsigned char *)newData, sizeof(newData) - sizeof(newData->ttlOrCrc));

#if DEBUG_MODE
    Serial.print(F("💾[SRAM storage: New data was added: "));
    Serial.print(storageIndex);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.println(F(" ✅]"));
#endif
}

void SRAM::clearAllPositions()
{
#if DEBUG_MODE
    Serial.println(F("💾[SRAM storage: clearing memory 🫙]"));
#endif
    storageIndex = 0;
    storageOverflow = false;
}
