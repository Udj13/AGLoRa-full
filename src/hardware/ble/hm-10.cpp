/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#include "../../settings/settings.h"

#if defined(ARDUINO_AVR_EBYTE_E32) || defined(ARDUINO_AVR_EBYTE_E220)

#include <Arduino.h>
#include "hm-10.h"


BLE::BLE()
{
}

void BLE::setup()
{
#if DEBUG_MODE && DEBUG_BLE
    Serial.print(F("📲[BLE: ready for work ✅. Maximum Transmission Unit (MTU) = "));
    Serial.print(MTU);
    Serial.println(F("]"));
#endif
#if !DEBUG_MODE
    sendCommand(F("AT"));
    sendCommand(F("AT+NAMEAGLoRa"));
    sendCommand(F("AT+ROLE0"));
#endif
}

String BLE::read()
{
    String result = "";
    while (Serial.available())
    {
        result += Serial.readString(); // read until timeout
    }
    result.trim(); // remove any \r \n whitespace at the end of the String

    return result;
}

void BLE::send(String *package)
{
#if DEBUG_MODE && DEBUG_BLE
    Serial.print(F("📲[BLE: 📫 Sending: "));
    Serial.print(*package);

    Serial.print(F("\t"));
    for (byte i = 1; i <= MTU; ++i)
    {
        Serial.print(i % 10);
    }
    Serial.print(F(" (MTU = "));
    Serial.print(MTU);
    Serial.println(F(")"));
#endif

    bool isStringNotEmpty = true;
    while (isStringNotEmpty)
    {
#if DEBUG_MODE && DEBUG_BLE
        Serial.print(F("\t"));
#endif
        String nextSendMTU = package->substring(0, MTU);
        package->remove(0, MTU);
        isStringNotEmpty = package->length() != 0;

        // important part
        Serial.print(nextSendMTU); //  here we send data to BLE
        delay(10);

#if DEBUG_MODE && DEBUG_BLE
        if (isStringNotEmpty)
            Serial.println(F(" ⮐"));
#endif
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

#endif