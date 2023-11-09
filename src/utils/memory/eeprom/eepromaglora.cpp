#include <Arduino.h>
#include "../../../settings/settings.h"
#include "eepromaglora.h"
#include "../../crc/crc.h"
#include <EEPROM.h>

EEPROMAglora::EEPROMAglora()
{
    EEPROMStorageSize = (EEPROM.length() - EEPROM_BEGIN_ADDRESS) / EEPROMDataSize;
}
 
void EEPROMAglora::setup()
{
    EEPROMStorageIndex = 0;
    incrementCounter = 0;

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("📀[EEPROM storage: Start EEPROM initialization. Size of memory: "));
    Serial.print(EEPROM.length());
    Serial.println(F(" bytes]"));
    Serial.print(F("📀[EEPROM storage: "));
    Serial.print(EEPROMStorageSize);
    Serial.println(F(" track points can be saved.]"));
    Serial.println(F("\t CRC check symbols: ✅ CRC OK, ⛔️ CRC ERROR, ⬜ empty memory cell, underlined CRC\u0332 - current cell"));
    Serial.print(F("Finding the index of the last record. Read memory... "));
#endif

    unsigned int prevIncCounter = 0;

    for (unsigned int i = 0; i < EEPROMStorageSize; i++)
    {
        EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

        if (EEPROMdata.counter == 255)
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

        if (abs(EEPROMdata.counter - prevIncCounter) > 1)
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
                    break;
                }
            }
        }
        prevIncCounter = EEPROMdata.counter;
    }

#if DEBUG_MODE && DEBUG_MEMORY
    Serial.println();
    Serial.print(F("Next record will be "));
    Serial.println(EEPROMStorageIndex);
#endif
}

bool EEPROMAglora::checkUnique(DATA *newPoint)
{
    return false;
}

/// @brief Save data to storage
/// @param newData that needs to be added to the storage
/// @return the index of added data 
unsigned int EEPROMAglora::save(DATA *newData)
{
  EEPROMdata.counter = incrementCounter;
    EEPROMdata.data = *newData;
    EEPROMdata.crc = calculateCRC((unsigned char*)newData, sizeof(DATA));
    EEPROM.put(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

    #if DEBUG_MODE && DEBUG_MEMORY
      Serial.print(F("📀[EEPROM storage: New data added. Address: "));
      Serial.print(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS);
      Serial.print(F(", index: "));
      Serial.print(EEPROMStorageIndex);
      Serial.print(F(", counter: "));
      Serial.print(EEPROMdata.counter);
      Serial.print(F(", CRC: "));
      Serial.print(EEPROMdata.crc);
      Serial.println(F("]"));
    #endif


    EEPROMStorageIndex++;
    if (EEPROMStorageIndex >= EEPROMStorageSize) {
      EEPROMStorageIndex = 0;
    }

    incrementCounter++;
    if ( incrementCounter >= 255 ) {
      incrementCounter = 0;
    }
  
    return incrementCounter;
}

DATA *EEPROMAglora::load(unsigned int index)
{
    return nullptr;
}

void EEPROMAglora::clearAllPositions()
{
      #if DEBUG_MODE 
      Serial.println(F("📀[EEPROM storage: clearing memory.]"));
    #endif

    unsigned char memoryCell = 0;
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.get(i + EEPROM_BEGIN_ADDRESS, memoryCell);
      if( memoryCell != 255 ){  // 255 - default value in EEPROM
        memoryCell = 255;
        EEPROM.put(i + EEPROM_BEGIN_ADDRESS, memoryCell);
      }
    }

    EEPROMStorageIndex = 0;
    incrementCounter = 0;
}



bool EEPROMAglora::checkCRC()
{



#if DEBUG_MODE && DEBUG_MEMORY
    for (unsigned int  i = 0; i < EEPROMStorageSize; i++)
    {
        EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

        unsigned char crc = calculateCRC((unsigned char *)&EEPROMdata.data, sizeof(EEPROMdata.data));

        Serial.print(F(", "));
        if ((i % 10) == 0)
        {
            Serial.println(F("*"));
        }
        Serial.print(EEPROMdata.counter);

        if (EEPROMdata.counter != 255)
        {
            if (crc == EEPROMdata.crc)
            {
                Serial.print(F("(CRC OK ✅)"));
            }
            else
            {
                Serial.print(F("(CRC ERROR ⛔️)"));
            }
        }
    }
    Serial.println();
#endif




#if DEBUG_MODE && DEBUG_MEMORY
    Serial.print(F("💾[SRAM storage: checking CRC, "));
    Serial.print(storageIndex);
    Serial.print(F("/"));
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.print(F(" cells are used, storageOverflow is "));
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
                ((i == 0)&&(storageOverwrite)))
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

bool EEPROMAglora::checkCRC(DATA *loraDataPacket)
{
    return false;
}

bool EEPROMAglora::checkCRC(unsigned int index)
{
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
    return false;
}
