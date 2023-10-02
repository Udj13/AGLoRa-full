#include <Arduino.h>
#include "ble.h"

// ================= BLE SECTION ===================

BLE::BLE(bool debugMode)
{
    _debugMode = debugMode;
}

void BLE::setup()
{
#if !DEBUG_MODE
    sendCommand(F("AT"));
    sendCommand(F("AT+NAMEAGLoRa"));
    sendCommand(F("AT+ROLE0"));
#endif
}

void BLE::read(void storageManager(String *command))
{
    while (Serial.available())
    {
        String command = Serial.readString(); // read until timeout
        command.trim();                       // remove any \r \n whitespace at the end of the String
        storageManager(&command);
    }
}

void BLE::sendCommand(const String command)
{
    Serial.println(command);
    delay(200); // wait some time
    while (Serial.available())
    {
        Serial.read();
    }
}
