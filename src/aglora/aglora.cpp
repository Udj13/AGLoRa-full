#include "aglora.h"
#include "../settings/settings.h"

AGLORA::AGLORA()
{
}

void AGLORA::hello()
{
#if DEBUG_MODE
  Serial.println(F("All initializations completed."));
  Serial.println();
  for (int i = 0; i < 25; i++)
  {
    Serial.print(F("-"));
    delay(100);
  }
  Serial.println();
  Serial.println(F("AGLORA tracker started..."));
#endif
}

/// Udpate sensors in loraDataPacket
void AGLORA::updateSensors(DATA *data)
{
  data->battery = 100; // just for example
}

/// Udpate GPS coordinates in loraDataPacket
/// return true is success (the coordinates are valid)
bool AGLORA::updateLocation(GPS *gps, DATA *loraDataPacket)
{
  gps->port.listen(); // switch to gps software serial

  bool newData = false;
  // For three seconds we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 3000;)
  {
    while (gps->port.available() > 0)
      if (gps->module.encode(gps->port.read()))
      {
        newData = true;
      }
  }
  digitalWrite(LED_BUILTIN, LOW);

  if (newData && gps->module.location.isValid() && gps->module.date.isValid() && gps->module.time.isValid())
  {
    digitalWrite(LED_BUILTIN, HIGH); // GPS is valid

    // data set
    loraDataPacket->lat = gps->module.location.lat();
    loraDataPacket->lon = gps->module.location.lng();
    loraDataPacket->sat = gps->module.satellites.value();

    loraDataPacket->year = gps->module.date.year() - 2000;
    loraDataPacket->month = gps->module.date.month();
    loraDataPacket->day = gps->module.date.day();

    loraDataPacket->hour = gps->module.time.hour();
    loraDataPacket->minute = gps->module.time.minute();
    loraDataPacket->second = gps->module.time.second();

    strcpy(loraDataPacket->name, NAME);

#if DEBUG_MODE
    gps->printGPSInfo();
#endif

    return true; // success
  }
  else
  {
#if DEBUG_MODE
    Serial.println();
    Serial.println(F("[GPS: No valid data.]"));
#endif
    return false;
  }

  if (gps->module.charsProcessed() < 10)
  {
#if DEBUG_MODE
    Serial.println();
    Serial.println(F("[GPS: No characters received from GPS, check wiring!]"));
#endif
    return false;
  }
}

// void AGLORA::request(String * request){
//   storageManager(request);
// };

void AGLORA::printPackage(DATA *loraDataPacket)
{
  // DEBUG_MODE
#if DEBUG_MODE // dump out what was just received
  Serial.println();
  Serial.println(F("[LoRa: New data received.]"));
  Serial.print(F("ID: "));
  Serial.print(loraDataPacket->name);
  Serial.print(F(", lat: "));
  Serial.print(loraDataPacket->lat, 6);
  Serial.print(F(", lon: "));
  Serial.print(loraDataPacket->lon, 6);
  Serial.print(F(", satellites: "));
  Serial.print(loraDataPacket->sat);

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
  Serial.println(F(" (UTC)"));

  Serial.print(F(", TTL: "));
  Serial.print(loraDataPacket->ttl);

  Serial.println();
#endif
}