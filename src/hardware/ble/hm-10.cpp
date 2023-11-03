#include <Arduino.h>
#include "hm-10.h"
#include "../../settings/settings.h"

// ================= BLE SECTION ===================

BLE_HM10::BLE_HM10()
{
}

void BLE_HM10::setup()
{
#if DEBUG_MODE
    Serial.print(F("📲[BLE: ready for work ✅. MTU = "));
    Serial.print(MTU);
    Serial.println(F("]"));
#else
    sendCommand(F("AT"));
    sendCommand(F("AT+NAMEAGLoRa"));
    sendCommand(F("AT+ROLE0"));
#endif
}

String BLE_HM10::read()
{
    String result = "";
    while (Serial.available())
    {
        result += Serial.readString(); // read until timeout
    }
    result.trim(); // remove any \r \n whitespace at the end of the String

    return result;
}

void BLE_HM10::send(String * package)
{
#if DEBUG_MODE
    Serial.println(F("📲[BLE: 📫 Sending:"));
    Serial.println(* package);
#endif

    bool isStringNotEmpty = true;
    while (isStringNotEmpty)
    {
#if DEBUG_MODE
        Serial.print(F("\tMTU: "));
#endif
        const String nextSendMTU = package->substring(0, MTU - 1);
        Serial.println(nextSendMTU);
#if DEBUG_MODE
        Serial.println(F("⮐"));
#endif
        isStringNotEmpty = package->length() != 0;
    }

#if DEBUG_MODE
    Serial.println(F(">>]"));
#endif
}

void BLE_HM10::sendCommand(const String command)
{
    Serial.println(command);
    delay(200); // wait some time
    while (Serial.available())
    {
        Serial.read();
    }
}
