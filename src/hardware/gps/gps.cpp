#include "gps.h"

// ================= GPS SECTION =================================

GPS::GPS(uint8_t pinRx, uint8_t pinTx,  long speed) : port(pinRx, pinTx)
{
    port.begin(speed);
    pinMode(LED_BUILTIN, OUTPUT); // GPS valid indicator
}

void GPS::setup()
{
#if DEBUG_MODE
    Serial.println(F("[GPS: Start GPS configuration.]"));
#endif
}

void GPS::displayGPSInfo()
{
#if DEBUG_MODE
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println();
    Serial.print(F("[GPS: updating my GPS position. Satellites in view: "));
    Serial.print(gps.satellites.value());
    Serial.println(F("]"));

    Serial.print(F("Location: "));

    if (gps.location.isValid())
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print(F("INVALID "));
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    }
    else
    {
        Serial.print(F("INVALID"));
    }
#endif
}