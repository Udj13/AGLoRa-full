#include "sram.h"

void SRAM::setup()
{
#if DEBUG_MODE
    Serial.println(F("💾[SRAM storage: memory is ready.]"));
#endif
}

/// @brief Checking new data for uniqueness
/// @param loraDataPacket
/// @return true if the same data is not exist
bool SRAM::checkUnique(DATA *loraDataPacket)
{
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

#if DEBUG_MODE
    Serial.println(F("[Storage: New data added.]"));
#endif
}

void SRAM::clearAllPositions()
{
#if DEBUG_MODE
    Serial.println(F("[Storage: clearing memory.]"));
#endif
    storageIndex = 0;
    storageOverflow = false;
}
