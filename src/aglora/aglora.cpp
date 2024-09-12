/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty

*/

#include <Arduino.h>
#include "aglora.h"

const String bleProtocolPrefix = "AGLoRa-";
const String bleProtocolTypePoint = "point";
const String bleProtocolTypeMemory = "memory";
const String bleProtocolVersion = "&ver=2.1";
const String bleProtocolParamCRC = "&crc=";
const String bleProtocolOK = "ok";
const String bleProtocolError = "error";

const String bleProtocolParamMemorySize = "&memsize=";
const String bleProtocolParamMemoryOverwrite = "&overwrite=";
const String bleProtocolParamMemoryIndex = "&index=";

const String bleProtocolDivider = "\r\n";

AGLORA::AGLORA(IMemory *memory, BLE_HM10 *ble)
{
  _ble = ble;
  _memory = memory;
}

void AGLORA::hello()
{
#if DEBUG_MODE && DEBUG_AGLORA
  Serial.println(F("[power on]"));

  Serial.print(F("Waiting | "));
  for (int i = 0; i < 50; i++)
  {
    Serial.print(F("#"));
    delay(50);
  }
  Serial.println();
  Serial.println(F("AGLORA tracker started..."));
#endif
}

/// @brief
/// 1. clear
/// 2. set name
/// 3. set default ttl
/// @param loraDataPacket
void AGLORA::clearDataPacket(DATA *trackerData)
{
  memset(trackerData, 0, sizeof(&trackerData));
  strcpy(trackerData->name, NAME);
#if DEBUG_MODE && DEBUG_AGLORA
  Serial.println(F("🟢[AGLoRa: time to send your location📍, new loraDataPacket prepared 📦]"));
#endif
}

void AGLORA::printPackage(LORADATA *loraDataPacket)
{
  // DEBUG_MODE
#if DEBUG_MODE && DEBUG_AGLORA // dump out what was just received
  Serial.println(F("🟢[AGLoRa: loraDataPacket now contains ↴]"));
  Serial.print(F("     Name: "));
  Serial.print(loraDataPacket->data.name);
  Serial.print(F(", 🌎 lat: "));
  Serial.print(loraDataPacket->data.lat, 6);
  Serial.print(F(", lon: "));
  Serial.print(loraDataPacket->data.lon, 6);

  if (loraDataPacket->data.gpsValid)
    Serial.print(F(", GPS 🆗"));
  else
    Serial.print(F(", GPS ❌"));

  // Serial.print(F(", sat: "));
  // Serial.print(loraDataPacket->data.sat);   // example
  // Serial.print(F(", hdop: "));
  // Serial.print(loraDataPacket->data.hdop);  // example

  Serial.print(F(", 📆 date: "));
  Serial.print(loraDataPacket->data.year);
  Serial.print(F("/"));
  if (loraDataPacket->data.month < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->data.month);
  Serial.print(F("/"));
  if (loraDataPacket->data.day < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->data.day);

  Serial.print(F(", 🕰️ time: "));
  Serial.print(loraDataPacket->data.hour);
  Serial.print(F(":"));
  if (loraDataPacket->data.minute < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->data.minute);
  Serial.print(F(":"));
  if (loraDataPacket->data.second < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->data.second);
  Serial.print(F(" (UTC)"));

  Serial.print(F(" 📦 TTL="));
  Serial.print(loraDataPacket->ttl);

  Serial.println();
#endif
}

void AGLORA::getRequest(String request)
{
  if (request.length() == 0)
  {
    return;
  }

#if DEBUG_MODE && DEBUG_AGLORA
  Serial.println();
  Serial.print(F("🟢[AGLoRa: 📭 BLE request received <<"));
  Serial.print(request);
  Serial.println(F(">> received]"));
  Serial.println();
#endif

  if (request.startsWith(F("crc")))
  {
    checkMemoryToBLE();
    return;
  }

  if (request.startsWith(F("clear")))
  {
    _memory->clearAllPositions();
    checkMemoryToBLE();
    return;
  }

  if (request.startsWith(F("all")))
  {
    sendAllPackagesToBLE();
    return;
  }

  if (request.startsWith(F("id")))
  {
    request.remove(0, 2);
    unsigned int index = request.toInt();
    sendPackageToBLEFromStorage(index);

    return;
  }
}

void AGLORA::checkMemoryToBLE()
{
  String response = bleProtocolPrefix +
                    bleProtocolTypeMemory +
                    bleProtocolVersion;
  response += bleProtocolParamCRC;
  response += _memory->checkCRC() ? bleProtocolOK : bleProtocolError;
  response += bleProtocolParamMemorySize + _memory->getSize();
  response += bleProtocolParamMemoryIndex + _memory->getIndex();
  response += bleProtocolParamMemoryOverwrite + _memory->getStorageOverwrite();
  response += bleProtocolDivider;
  _ble->send(&response);
}

void AGLORA::sendPackageToBLE(DATA *trackerData, int index)
{
  String response = bleProtocolPrefix +
                    bleProtocolTypePoint +
                    bleProtocolVersion;

  response += sendToPhone(trackerData);
  response += bleProtocolParamMemoryIndex + index;
  response += bleProtocolParamCRC;
  response += _memory->checkCRC(index) ? bleProtocolOK : bleProtocolError;
  response += bleProtocolDivider;

#if DEBUG_MODE && DEBUG_AGLORA
  Serial.print(F("🟢AGLoRa: send point 📦 to BLE → "));
  Serial.print(response);
#endif

  _ble->send(&response);
}

void AGLORA::sendAllPackagesToBLE()
{
  unsigned int maxIndex = _memory->getStorageOverwrite() ? _memory->getSize() : _memory->getIndex();
  for (unsigned int i = 0; i < maxIndex; ++i)
  {
#if DEBUG_MODE && DEBUG_AGLORA
    Serial.print(F("🟢[AGLoRa: loading "));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.print(maxIndex);
    Serial.print(F(" 📦 from memory ]"));
#endif

    sendPackageToBLE(_memory->load(i), i);
  }

#if DEBUG_MODE && DEBUG_AGLORA
  Serial.println();
#endif
}

void AGLORA::sendPackageToBLEFromStorage(unsigned int index)
{
#if DEBUG_MODE && DEBUG_AGLORA
  Serial.print(F("🟢[AGLoRa: loading 📦  from index "));
  Serial.print(index);
  Serial.print(F("]"));
#endif

  if ((_memory->getStorageOverwrite() == false) && (_memory->getIndex() == 0))
  {
#if DEBUG_MODE && DEBUG_AGLORA
    Serial.println(F("- error 🚨 empty memory 🚨"));
#endif

    String response = bleProtocolPrefix +
                      bleProtocolTypeMemory +
                      bleProtocolVersion;
    response += bleProtocolParamMemorySize + _memory->getSize();
    response += bleProtocolParamMemoryIndex + _memory->getIndex();
    response += bleProtocolParamMemoryOverwrite + _memory->getStorageOverwrite();
    response += bleProtocolDivider;
    _ble->send(&response);

    return;
  }

  unsigned int maxIndex = _memory->getStorageOverwrite() ? _memory->getSize() : _memory->getIndex();
  if (index > maxIndex - 1)
  {
#if DEBUG_MODE && DEBUG_AGLORA
    Serial.println(F("- error 🚨 index out of range 🚨"));
#endif
    return;
    // TODO: send error
  }

  sendPackageToBLE(_memory->load(index), index);
}