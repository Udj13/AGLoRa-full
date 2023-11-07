#include "gps.h"

// ================= GPS SECTION =================================

GPS::GPS(uint8_t pinRx, uint8_t pinTx, long speed, uint8_t ledPin) : gpsPort(pinRx, pinTx),
                                                                     gpsModule()
{
    gpsPort.begin(speed);
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT); // GPS valid indicator
}

void GPS::setup()
{
#if DEBUG_MODE && DEBUG_GPS
    Serial.println(F("📡[GPS: Start GPS configuration.]"));
#endif
}

void GPS::printGPSInfo()
{
#if DEBUG_MODE && DEBUG_GPS
    Serial.print(F(" Satellites in view: "));
    Serial.print(gpsModule.satellites.value());
    Serial.print(F("🛰️. HDOP: "));
    Serial.print(gpsModule.hdop.value());
    Serial.println(F("]"));

    Serial.print(F("📍 Location: "));
    if (gpsModule.location.isValid())
    {
        Serial.print(gpsModule.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gpsModule.location.lng(), 6);
    }
    else
    {
        Serial.print(F("❌ INVALID "));
        Serial.print(gpsModule.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gpsModule.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gpsModule.location.lng(), 6);
    }

    Serial.print(F(" 🗓️ Date/Time: "));
    if (gpsModule.date.isValid())
    {
        Serial.print(gpsModule.date.month());
        Serial.print(F("/"));
        Serial.print(gpsModule.date.day());
        Serial.print(F("/"));
        Serial.print(gpsModule.date.year());
    }
    else
    {
        Serial.print(F("❌ INVALID"));
    }

    Serial.print(F(" ⏰ "));
    if (gpsModule.time.isValid())
    {
        if (gpsModule.time.hour() < 10)
            Serial.print(F("0"));
        Serial.print(gpsModule.time.hour());
        Serial.print(F(":"));
        if (gpsModule.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(gpsModule.time.minute());
        Serial.print(F(":"));
        if (gpsModule.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(gpsModule.time.second());
        Serial.print(F("."));
        if (gpsModule.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.println(gpsModule.time.centisecond());
    }
    else
    {
        Serial.println(F("INVALID"));
    }
#endif
}

void GPS::printReadingIndication(unsigned long start, unsigned int delay)
{
#if DEBUG_MODE && DEBUG_GPS
    byte progress = (10 * (millis() - start)) / delay;
    if (progress != _readingIndicator)
    {
        _readingIndicator = progress;
        Serial.print(F("#"));
    }
#endif
}

void GPS::updateLocation(DATA *loraDataPacket)
{

#if DEBUG_MODE && DEBUG_GPS
    Serial.print(F("📡[GPS reading: "));
#endif

    gpsPort.listen(); // switch to gps software serial

    bool newData = false;
    // For three seconds we parse GPS data and report some key values
    const unsigned int readingDelay = 3000;
    for (unsigned long start = millis(); millis() - start < readingDelay;)
    {
        printReadingIndication(start, readingDelay);
        while (gpsPort.available() > 0)
            if (gpsModule.encode(gpsPort.read()))
            {
                newData = true;
            }
    }
    turnIndicatorOff();

    if (newData && gpsModule.location.isValid() && gpsModule.date.isValid() && gpsModule.time.isValid())
    {
        turnIndicatorOn(); // GPS is valid

        // data set
        loraDataPacket->lat = gpsModule.location.lat();
        loraDataPacket->lon = gpsModule.location.lng();
        loraDataPacket->sat = gpsModule.satellites.value();
        loraDataPacket->hdop = gpsModule.hdop.value() / 10;

        loraDataPacket->year = gpsModule.date.year() - 2000;
        loraDataPacket->month = gpsModule.date.month();
        loraDataPacket->day = gpsModule.date.day();

        loraDataPacket->hour = gpsModule.time.hour();
        loraDataPacket->minute = gpsModule.time.minute();
        loraDataPacket->second = gpsModule.time.second();

        strcpy(loraDataPacket->name, NAME);

        printGPSInfo();
        return;
    }
    else
    {
#if DEBUG_MODE && DEBUG_GPS
        Serial.println(F("❌ No valid data.]"));
#endif
        return;
    }

    if (gpsModule.charsProcessed() < 10)
    {
#if DEBUG_MODE && DEBUG_GPS
        Serial.println(F("❌ No characters received from GPS, check wiring!]"));
#endif
        return;
    }
}

void GPS::turnIndicatorOn()
{
    digitalWrite(_ledPin, HIGH);
}

void GPS::turnIndicatorOff()
{
    digitalWrite(_ledPin, LOW);
}
