#include "aglora.h"
#include "../settings/settings.h"

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

AGLORA::AGLORA(SRAM *memory, BLE_HM10 *ble)
{
  _ble = ble;
  _memory = memory;
}

void AGLORA::hello()
{
#if DEBUG_MODE
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
/// 3. set ttl
/// @param loraDataPacket
void AGLORA::clearDataPacket(DATA *loraDataPacket)
{
  memset(loraDataPacket, 0, sizeof(&loraDataPacket));
  strcpy(loraDataPacket->name, NAME);
  loraDataPacket->ttlOrCrc = TTL;
#if DEBUG_MODE
  Serial.println(F("🟢[AGLoRa: time to send your location📍, new loraDataPacket prepared 📦]"));
#endif
}

// void AGLORA::request(String * request){
//   storageManager(request);
// };

void AGLORA::printPackage(DATA *loraDataPacket)
{
  // DEBUG_MODE
#if DEBUG_MODE // dump out what was just received
  Serial.println(F("🟢[AGLoRa: loraDataPacket now contains ↴]"));
  Serial.print(F("     Name: "));
  Serial.print(loraDataPacket->name);
  Serial.print(F(", lat: "));
  Serial.print(loraDataPacket->lat, 6);
  Serial.print(F(", lon: "));
  Serial.print(loraDataPacket->lon, 6);
  Serial.print(F(", sat: "));
  Serial.print(loraDataPacket->sat);
  Serial.print(F(", hdop: "));
  Serial.print(loraDataPacket->hdop);

  Serial.print(F(", date: "));
  Serial.print(loraDataPacket->year);
  Serial.print(F("/"));
  if (loraDataPacket->month < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->month);
  Serial.print(F("/"));
  if (loraDataPacket->day < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->day);

  Serial.print(F(", time: "));
  Serial.print(loraDataPacket->hour);
  Serial.print(F(":"));
  if (loraDataPacket->minute < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->minute);
  Serial.print(F(":"));
  if (loraDataPacket->second < 10)
    Serial.print(F("0"));
  Serial.print(loraDataPacket->second);
  Serial.print(F(" (UTC)"));

  Serial.print(F(", TTL: "));
  Serial.print(loraDataPacket->ttlOrCrc);

  Serial.println();
#endif
}

void AGLORA::getRequest(String request)
{
  if (request.length() == 0)
  {
    return;
  }

#if DEBUG_MODE
  Serial.println();
  Serial.print(F("🟢[AGLoRa: 📭 BLE request received <<"));
  Serial.print(request);
  Serial.println(F(">> received]"));
  Serial.println();
#endif

  if (request.startsWith(F("crc")))
  {
    checkMemory();
    return;
  }

  if (request.startsWith(F("clear")))
  {
    _memory->clearAllPositions();
    checkMemory();
    return;
  }

  if (request.startsWith(F("all")))
  {
    sendAllPackagesToBLE();
    return;
  }

  if (request.startsWith(F("id")))
  {
    request.remove(0,2);
    unsigned int index = request.toInt();
    sendPackageToBLEFromStorage(index);

    return;
  }


}

void AGLORA::checkMemory()
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

void AGLORA::sendPackageToBLE(DATA *loraDataPacket, int index)
{
  String response = bleProtocolPrefix +
                    bleProtocolTypePoint +
                    bleProtocolVersion;

  response += sendToPhone(loraDataPacket);
  response += bleProtocolParamMemoryIndex + index;
  response += bleProtocolParamCRC;
  response += _memory->checkCRC(index) ? bleProtocolOK : bleProtocolError;
  response += bleProtocolDivider;

#if DEBUG_MODE
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
#if DEBUG_MODE
    Serial.print(F("🟢[AGLoRa: loading "));
    Serial.print(i+1);
    Serial.print(F("/"));
    Serial.print(maxIndex);
    Serial.print(F(" 📦 from memory ]"));
#endif

    sendPackageToBLE(_memory->load(i), i);
  }

#if DEBUG_MODE
    Serial.println();
#endif

}


void AGLORA::sendPackageToBLEFromStorage(unsigned int index){
#if DEBUG_MODE
    Serial.print(F("🟢[AGLoRa: loading 📦  from index "));
    Serial.print(index);
    Serial.print(F("]"));
#endif


if((_memory->getStorageOverwrite() == false)&&( _memory->getIndex()==0)){
#if DEBUG_MODE
    Serial.println(F("- error 🚨 empty memory 🚨"));
#endif
    return;
    // TODO: send error
}


  unsigned int maxIndex = _memory->getStorageOverwrite() ? _memory->getSize() : _memory->getIndex();
  if(index > maxIndex-1) {
#if DEBUG_MODE
    Serial.println(F("- error 🚨 index out of range 🚨"));
#endif
    return;
    // TODO: send error
  }

  sendPackageToBLE(_memory->load(index), index); 
}