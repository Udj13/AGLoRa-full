#include "gps.h"
#include "../../settings/settings.h"

// ================= GPS SECTION =================================

GPS::GPS(uint8_t pinRx, uint8_t pinTx, long speed) : port(pinRx, pinTx),
                                                     tinygps()
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

void GPS::listen()
{
    port.listen();
}

void GPS::printGPSInfo()
{
#if DEBUG_MODE
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println();
    Serial.print(F("[GPS: updating my GPS position. Satellites in view: "));
    Serial.print(tinygps.satellites.value());
    Serial.println(F("]"));

    Serial.print(F("Location: "));

    if (tinygps.location.isValid())
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print(tinygps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(tinygps.location.lng(), 6);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print(F("INVALID "));
        Serial.print(tinygps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(tinygps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(tinygps.location.lng(), 6);
    }

    Serial.print(F("  Date/Time: "));
    if (tinygps.date.isValid())
    {
        Serial.print(tinygps.date.month());
        Serial.print(F("/"));
        Serial.print(tinygps.date.day());
        Serial.print(F("/"));
        Serial.print(gtinygpsps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (tinygps.time.isValid())
    {
        if (tinygps.time.hour() < 10)
            Serial.print(F("0"));
        Serial.print(tinygps.time.hour());
        Serial.print(F(":"));
        if (tinygps.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(tinygps.time.minute());
        Serial.print(F(":"));
        if (tinygps.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(tinygps.time.second());
        Serial.print(F("."));
        if (tinygps.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.print(gptinygpss.time.centisecond());
    }
    else
    {
        Serial.print(F("INVALID"));
    }
#endif
}