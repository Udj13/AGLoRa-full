#include "tests.h"

bool TESTS::hasNewDataEveryXSec(DATA *loraDataPacket, GPS *gps, byte interval)
{
        int _intervarSec = interval * 1000;
        if (_timeOfLastSendedPacket + _intervarSec < millis())
        {
#if DEBUG_MODE
                Serial.println();
                Serial.println(F("💛💛💛 [TEST: virtual tracker transmitted the data] 💛💛💛"));
#endif

                gps->updateLocation(loraDataPacket); // we need an actual time
                strcpy(loraDataPacket->name, OTHER_NAME);
                loraDataPacket->lat = 45.455631;
                loraDataPacket->lon = 54.084960;
                loraDataPacket->ttlOrCrc = TTL;

#if DEBUG_MODE
                Serial.println();
#endif

                _timeOfLastSendedPacket = millis();
                return true;
        }

        return false;
}
