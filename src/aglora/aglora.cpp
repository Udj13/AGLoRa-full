#include "aglora.h"
#include "../settings/settings.h"

const String bleProtocolPrefix = "AGLoRa";
const String bleProtocolVersion = "&ver=2.1";
const String bleProtocolOK = "ok";
const String bleProtocolError = "error";
const String bleProtocolCRCMemory = "&crcmemory=";
const String bleProtocolDivider = "\r\n";


AGLORA::AGLORA(SRAM memory, BLE_HM10 ble)
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
  Serial.print(loraDataPacket->month);
  Serial.print(F("/"));
  Serial.print(loraDataPacket->day);

  Serial.print(F(", time: "));
  Serial.print(loraDataPacket->hour);
  Serial.print(F(":"));
  Serial.print(loraDataPacket->minute);
  Serial.print(F(":"));
  Serial.print(loraDataPacket->second);
  Serial.print(F(" (UTC)"));

  Serial.print(F(", TTL: "));
  Serial.print(loraDataPacket->ttlOrCrc);

  Serial.println();
#endif
}

void AGLORA::request(String request)
{
  if (request.length() == 0)
  {
    return;
  }
#if DEBUG_MODE
  Serial.println();
  Serial.print(F("🟢[AGLoRa: request <<"));
  Serial.print(request);
  Serial.println(F(">> received]"));
  Serial.println();
#endif

  if (request.startsWith(F("checkmem")))
  {
    checkMemory();
    return;
  }
}

void AGLORA::checkMemory()
{
    // String response = bleProtocolPrefix + bleProtocolVersion;
    // response += bleProtocolCRCMemory;
    // if(memory.checkCRC()){
    //     response += bleProtocolOK;
    // } else {
    //   response += bleProtocolError;
    // }
    // _ble.send(response);

}
