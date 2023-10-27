#include "tests.h"

bool TESTS::hasNewDataEvery60sec(DATA *loraDataPacket, GPS *gps)
{
    if (_timeOfLastSendedPacket + TEST_DATA_INTERVAL < millis())
    {
#if DEBUG_MODE
        Serial.println();
        Serial.println(F("💛💛💛 [TEST: virtual tracker transmitted the data] 💛💛💛"));
#endif

        gps->updateLocation(loraDataPacket); // we need an actual time
        strcpy(loraDataPacket->name, OTHER_NAME);
        loraDataPacket->lat = 45.455631;
        loraDataPacket->lon = 54.084960;

#if DEBUG_MODE
        Serial.println();
#endif

        _timeOfLastSendedPacket = millis();
        return true;
    }

    return false;
}
