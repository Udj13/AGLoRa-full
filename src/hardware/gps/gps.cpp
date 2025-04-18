/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty

*/

#include "gps.h"

// ================= GPS SECTION =================================


#if USE_HARDWARE_GPS_UART
    const int gps_hardware_uart = 0; // hardware port
#endif

GPS::GPS(uint8_t pinRx, uint8_t pinTx, long speed, INDICATION *indication) 
                                                        #if USE_HARDWARE_GPS_UART                    
                                                            :gpsPort(gps_hardware_uart) //  hardware port
                                                        #else
                                                            : gpsPort(pinTx, pinRx) // software port
                                                        #endif
{

    #if USE_HARDWARE_GPS_UART                    
        gpsPort.begin(speed, SERIAL_8N1, pinTx, pinRx);  // hardware port
    #else
        gpsPort.begin(speed); // software port
    #endif
    _indication = indication;
}
void GPS::setup()
{
    #if DEBUG_MODE && DEBUG_GPS
        Serial.println(F("📡[GPS: Start GPS configuration.]"));
    #endif
}

void GPS::printReadingIndication(unsigned long start, unsigned int delay)
{
    #if DEBUG_MODE && DEBUG_GPS
        byte progress = (10 * (millis() - start)) / delay;
        if (progress != _readingIndicator)
        {
            _readingIndicator = progress;
            Serial.print(F("⏳"));
        }
    #endif
}

void GPS::updateLocation(DATA *dataPackage)
{
    #if DEBUG_MODE && DEBUG_GPS
        Serial.print(F("📡[GPS reading: "));
    #endif

    char c;
    String sentence = "";

    // For three seconds we parse GPS data and report some key values
    const unsigned int readingDelay = 3000;
    bool gpsReadingComplited = false;
    bool ggaCatched = false;
    bool rmcCatched = false;

    #if !USE_HARDWARE_GPS_UART
        gpsPort.listen(); // only for software - disable for hardware port
    #endif

    for (unsigned long start = millis(); millis() - start < readingDelay;)
    {
        printReadingIndication(start, readingDelay);
        while (gpsPort.available() > 0)
        {
            c = gpsPort.read();
            #if DEBUG_MODE && DEBUG_GPS && DEBUG_PRINT_FULL_GPS_LOG
                Serial.print(c);
            #endif
            if (c == '$')
            {
                // Start of a new sentence
                sentence = "";
            }
            else if (c == '\n')
            {
                // End of a sentence process it
                if (processRMCSentence(sentence, dataPackage))
                    rmcCatched = true;
                if (processGGASentence(sentence, dataPackage))
                    ggaCatched = true;

                if (ggaCatched && rmcCatched)
                {
                    gpsReadingComplited = true;
                    break;
                }
            }
            else
            {
                // Add character to the sentence
                sentence += c;
            }
        }
        if (gpsReadingComplited)
            break;
    }

    if (dataPackage->gpsValid)
    {
        #if DEBUG_MODE && DEBUG_GPS
                Serial.println(" ✅ GPS data is correct.]");
        #endif
        _indication->gps(GPSStatuses::correct); // GPS is valid
    }
    else
    {
        #if DEBUG_MODE && DEBUG_GPS
                Serial.println(F("❌ No valid data.]"));
        #endif
        _indication->gps(GPSStatuses::invalid); // GPS is invalid
    }
}


// Process RMC without checksum
//
// bool GPS::processRMCSentence(String sentence, DATA *dataPackage)
// {

//     if (sentence.startsWith("GPRMC"))
//     {

// #if DEBUG_MODE && DEBUG_GPS
//         Serial.print("✨");
// #endif

//         // Process RMC sentence
//         int commaIndex = sentence.indexOf(',');
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String status = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String latStr = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String latDir = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String lonStr = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String lonDir = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String speedKnots = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         // String course = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String dateStr = sentence.substring(0, commaIndex);

//         if (status == "A")
//         {
//             // Convert date to year, month, and day
//             dataPackage->day = dateStr.substring(0, 2).toInt();
//             ;
//             dataPackage->month = dateStr.substring(2, 4).toInt();
//             dataPackage->year = dateStr.substring(4, 6).toInt();

//             return true;
//         }
//     }

//     return false;
// }


// Process GGA without checksum
//
// bool GPS::processGGASentence(String sentence, DATA *dataPackage)
// {

//     if (sentence.startsWith("GPGGA"))
//     {

// #if DEBUG_MODE && DEBUG_GPS
//         Serial.print("✨");
// #endif

//         // Process GGA sentence
//         int commaIndex = sentence.indexOf(',');
//         String timeStr = sentence.substring(commaIndex + 1, commaIndex + 7);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String latStr = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String latDir = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String lonStr = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String lonDir = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String fixQuality = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String satStr = sentence.substring(0, commaIndex);
//         sentence = sentence.substring(commaIndex + 1);

//         commaIndex = sentence.indexOf(',');
//         String hdopStr = sentence.substring(0, commaIndex);

//         if (fixQuality.toInt() > 0)
//         {
//             dataPackage->gpsValid = true;
//             dataPackage->lat = latStr.toFloat();
//             dataPackage->lon = lonStr.toFloat();
//             //dataPackage->sat = satStr.toInt();        // optional
//             //dataPackage->hdop = hdopStr.toFloat();    // optional

//             // Convert latitude and longitude to decimal degrees
//             dataPackage->lat = convertDegMinToDecDeg(dataPackage->lat);
//             dataPackage->lon = convertDegMinToDecDeg(dataPackage->lon);
//             if (latDir == "S")
//                 dataPackage->lat = -dataPackage->lat;
//             if (lonDir == "W")
//                 dataPackage->lon = -dataPackage->lon;

//             // Convert time to hours, minutes, and seconds
//             int hour = timeStr.substring(0, 2).toInt();
//             int minute = timeStr.substring(2, 4).toInt();
//             int second = timeStr.substring(4, 6).toInt();
//             dataPackage->hour = hour;
//             dataPackage->minute = minute;
//             dataPackage->second = second;

//             return true;
//         }
//     }

//     return false;
// }


bool GPS::processRMCSentence(String sentence, DATA *dataPackage)
{
    if ((sentence.startsWith(F("GPRMC")) || sentence.startsWith(F("GNRMC")) || 
    sentence.startsWith(F("GLRMC")) || sentence.startsWith(F("GARMC")) || 
    sentence.startsWith(F("GBRMC")) || sentence.startsWith(F("GVRMC"))) 
    && sentence.indexOf('*') > 0)
    {
        sentence.trim();

        int asteriskPos = sentence.indexOf('*');
        uint8_t calculatedChecksum = 0;

        for (int i = 0; i < asteriskPos; i++) {
            calculatedChecksum ^= (uint8_t)sentence[i]; 
        }   

        String receivedChecksumStr = sentence.substring(asteriskPos + 1);
        uint8_t receivedChecksum = strtoul(receivedChecksumStr.c_str(), NULL, 16);

        if (calculatedChecksum != receivedChecksum) {

        #if DEBUG_MODE && DEBUG_GPS
            Serial.print(F("Bad checksum for: "));
            Serial.println(sentence);
        #endif
        
        return false;
    }


        #if DEBUG_MODE && DEBUG_GPS
            Serial.println(F("    ↳ RMC NMEA checksum: ✅"));
        #endif

        // Process RMC sentence
        int commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut GPRMC

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut UTC

        commaIndex = sentence.indexOf(',');
        String status = sentence.substring(0, commaIndex); // Position status (A = data valid, V = data invalid)
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut lat

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut lat dir (N = North, S = South)

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1);// cut lon

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut lon dir (E = East, W = West)

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut speed,  knots

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1); // cut

        commaIndex = sentence.indexOf(',');
        String dateStr = sentence.substring(0, commaIndex); // cut date: dd/mm/yy

        #if DEBUG_MODE && DEBUG_GPS
            // Serial.print(F("Status: "));
            // Serial.print(status);
            // Serial.print(F(" | Date: "));
            // Serial.println(dateStr);
        #endif


        if (status == F("A"))
        {
            // Convert date to year, month, and day
            dataPackage->day = dateStr.substring(0, 2).toInt();
            dataPackage->month = dateStr.substring(2, 4).toInt();
            dataPackage->year = dateStr.substring(4, 6).toInt();

            return true;
        }
    }

    return false;
}

bool GPS::processGGASentence(String sentence, DATA *dataPackage)
{
    if ((sentence.startsWith(F("GPGGA")) || sentence.startsWith(F("GNGGA")) || 
    sentence.startsWith(F("GLGGA")) || sentence.startsWith(F("GAGGA")) || 
    sentence.startsWith(F("GBGGA")) || sentence.startsWith(F("GXGGA"))) 
    && sentence.indexOf('*') > 0)
    {
        sentence.trim();

        int asteriskPos = sentence.indexOf('*');
        uint8_t calculatedChecksum = 0;
        
        for (int i = 0; i < asteriskPos; i++) {
            calculatedChecksum ^= (uint8_t)sentence[i]; 
        }   

        String receivedChecksumStr = sentence.substring(asteriskPos + 1);
        uint8_t receivedChecksum = strtoul(receivedChecksumStr.c_str(), NULL, 16);

        if (calculatedChecksum != receivedChecksum) {
            #if DEBUG_MODE && DEBUG_GPS
                Serial.print(F("❌   ↳ Bad checksum for GPGGA: "));
                Serial.println(sentence);
            #endif
            return false;
        }

        #if DEBUG_MODE && DEBUG_GPS
            Serial.println(F("    ↳ GGA NMEA checksum: ✅"));
        #endif

        // Process GGA sentence
        int commaIndex = sentence.indexOf(',');
        String timeStr = sentence.substring(commaIndex + 1, commaIndex + 7);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String latStr = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String latDir = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String lonStr = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String lonDir = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String fixQuality = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String satStr = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String hdopStr = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);

        commaIndex = sentence.indexOf(',');
        String altitudeStr = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);
         
        commaIndex = sentence.indexOf(',');
        String units = sentence.substring(0, commaIndex);
        sentence = sentence.substring(commaIndex + 1);
 
        if (fixQuality.toInt() > 0)
        {
            dataPackage->gpsValid = true;
            dataPackage->lat = latStr.toFloat();
            dataPackage->lon = lonStr.toFloat();
            dataPackage->sats = satStr.toInt();        // optional
            //dataPackage->hdop = hdopStr.toFloat();    // optional

            // Convert latitude and longitude to decimal degrees
            dataPackage->lat = convertDegMinToDecDeg(dataPackage->lat);
            dataPackage->lon = convertDegMinToDecDeg(dataPackage->lon);
            if (latDir == F("S"))
                dataPackage->lat = -dataPackage->lat;
            if (lonDir == F("W"))
                dataPackage->lon = -dataPackage->lon;

            // Convert time to hours, minutes, and seconds
            int hour = timeStr.substring(0, 2).toInt();
            int minute = timeStr.substring(2, 4).toInt();
            int second = timeStr.substring(4, 6).toInt();
            dataPackage->hour = hour;
            dataPackage->minute = minute;
            dataPackage->second = second;

            if (units == F("M")) {
                dataPackage->altitude = altitudeStr.toFloat();
            } else {
                dataPackage->altitude = 0.0f;
            #if DEBUG_MODE && DEBUG_GPS
                Serial.print(F("⚠ Unknown altitude units: "));
                Serial.println(units);
            #endif
            }
    
            return true;
        }
    }

    return false;
}

float GPS::convertDegMinToDecDeg(float degMin)
{
    float min = fmod(degMin, 100.0);
    float deg = (degMin - min) / 100.0;
    return deg + (min / 60.0);
}

// ========================= TinyGPSPlus version =============================
// https://github.com/mikalhart/TinyGPSPlus

//TinyGPSPlus gps;

// void GPS::updateLocation(DATA *dataPackage)
// {

// #if DEBUG_MODE && DEBUG_GPS
//     Serial.print(F("📡[GPS reading: "));
// #endif

//     gpsPort.listen(); // switch to gps software serial

//     bool newData = false;
//     // For three seconds we parse GPS data and report some key values
//     const unsigned int readingDelay = 1000;
//     for (unsigned long start = millis(); millis() - start < readingDelay;)
//     {
//         printReadingIndication(start, readingDelay);
//         while (gpsPort.available() > 0)
//             if (gpsModule.encode(gpsPort.read()))
//             {
//                 newData = true;
//             }
//     }

//     if (newData && gpsModule.location.isValid() && gpsModule.date.isValid() && gpsModule.time.isValid())
//     {
//         // data set
//         dataPackage->lat = gpsModule.location.lat();
//         dataPackage->lon = gpsModule.location.lng();
//         dataPackage->sat = gpsModule.satellites.value();
//         dataPackage->hdop = gpsModule.hdop.value() / 10;

//         dataPackage->year = gpsModule.date.year() - 2000;
//         dataPackage->month = gpsModule.date.month();
//         dataPackage->day = gpsModule.date.day();

//         dataPackage->hour = gpsModule.time.hour();
//         dataPackage->minute = gpsModule.time.minute();
//         dataPackage->second = gpsModule.time.second();

//         strcpy(dataPackage->name, NAME);

//         printGPSInfo();
//         _indication->gps(GPSStatuses::correct); // GPS is valid
//         return;
//     }
//     else
//     {
// #if DEBUG_MODE && DEBUG_GPS
//         Serial.println(F("❌ No valid data.]"));
// #endif
//         _indication->gps(GPSStatuses::invalid); // GPS is valid
//         return;
//     }

//     if (gpsModule.charsProcessed() < 10)
//     {
// #if DEBUG_MODE && DEBUG_GPS
//         Serial.println(F("❌ No characters received from GPS, check wiring!]"));
// #endif
//         _indication->gps(GPSStatuses::connectionError); // GPS is valid
//         return;
//     }
// }
