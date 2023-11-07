#include "eepromaglora.h"
#include "../../crc.h"
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
    Serial.print(F("📀[Storage: Start EEPROM initialization. Size of memory: "));
    Serial.print(EEPROM.length());
    Serial.println(F(" bytes]"));
    Serial.print(F("📀[Storage: "));
    Serial.print(EEPROMStorageSize);
    Serial.println(F(" track points can be saved.]"));
    Serial.print(F("Finding the index of the last record. Read memory... "));
#endif

    unsigned int prevIncCounter = 0;

#if DEBUG_MODE && DEBUG_MEMORY
    for (int i = 0; i < EEPROMStorageSize; i++)
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
                Serial.print(F("(CRC OK)"));
            }
            else
            {
                Serial.print(F("(CRC ERROR)"));
            }
        }
    }
    Serial.println();
#endif

    for (int i = 0; i < EEPROMStorageSize; i++)
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
