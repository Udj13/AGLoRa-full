#include <Arduino.h>

class SRAM
{
public:
    void storageInit();
    void writeTrackerInfoToBLE();
    void addNewDataToStorage(DATA *newData);
    void writeNewPositionToBLE();
    void writeAllPositionsToBLE();
    void writePositionToBLE(String *command);
    void clearAllPositions();

private:
    DATA storage[SRAM_STORAGE_SIZE];
    unsigned int storageIndex = 0;
    bool storageOverflow = false;

}


void SRAM::storageInit()
{
#if DEBUG_MODE
    Serial.println(F("[Storage: Start initialization.]"));
#endif
}

void SRAM::writeTrackerInfoToBLE()
{
#if DEBUG_MODE
    Serial.println();
    Serial.println(F("[LoRa: No new LORA data]"));
    Serial.println(F("[Bluetooth output: Send AGLoRa tracker's info.]"));
#endif

    Serial.print(F("AGLoRa-info")); // signature 6 bytes
    Serial.print(F("&ver=2.0"));    // BLE protocol version
    Serial.print(F("&myId="));      // tracker's name
    Serial.print(MY_NAME);
    Serial.print(F("&memorySize=")); // storage length
    Serial.print(SRAM_STORAGE_SIZE);
    Serial.print(F("&memoryIndex=")); // storage length
    Serial.println(storageIndex);
}

// ADD NEW DATA TO STORAGE
void SRAM::addNewDataToStorage(DATA *newData)
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

void SRAM::writeNewPositionToBLE()
{
#if DEBUG_MODE
    Serial.println("[Bluetooth output: Send new data to BLE.]");
#endif

    Serial.print(F("AGLoRa-newpoint&"));
    Serial.print(F("id="));
    Serial.print(storageIndex);
    sendPackageToBluetooth(&storage[storageIndex]);
}

// Send all data from storage to BLE
void BLE::writeAllPositionsToBLE()
{
#if DEBUG_MODE
    Serial.println(F("[Bluetooth output: Response to <all> command.]"));
#endif

    unsigned int maxIndex = storageOverflow ? (SRAM_STORAGE_SIZE - 1) : storageIndex;

    for (int i = 0; i <= maxIndex; i++)
    {

        Serial.print(F("AGLoRa-point&"));
        Serial.print(F("id="));
        Serial.print(i);
        Serial.print(F("&"));
        sendPackageToBluetooth(&storage[i]); // Next tracker DATA
        delay(10);
    }
}

void BLE::writePositionToBLE(String *command)
{
    int index = command->substring(3).toInt(); // length of command "id="

#if DEBUG_MODE
    Serial.println(F("[Bluetooth output: Response to <id=X> command.]"));
#endif

    if (index == 0)
    {
        Serial.println(F("ErrorZeroIndex"));
        return;
    }
    if (index > storageIndex)
    {
        Serial.println(F("ErrorIndexOutOfRange"));
        return;
    }

    Serial.print(F("AGLoRa-point&"));
    Serial.print(F("id="));
    Serial.print(index);
    Serial.print(F("&"));
    sendPackageToBluetooth(&storage[index]); // Next tracker DATA
    delay(10);
}

void BLE::clearAllPositions()
{
#if DEBUG_MODE
    Serial.println(F("[Storage: clearing memory.]"));
#endif

    storageIndex = 0;
    storageOverflow = false;
}
