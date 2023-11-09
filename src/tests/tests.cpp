#include "tests.h"

bool TESTS::hasNewDataEveryXSec(LORADATA *loraDataPacket, GPS *gps, byte interval)
{
        int _intervarSec = interval * 1000;
        if (_timeOfLastSendedPacket + _intervarSec < millis())
        {
#if DEBUG_MODE
                Serial.println();
                Serial.println(F("💛💛💛 [TEST: virtual tracker transmitted the data] 💛💛💛"));
#endif

                gps->updateLocation(loraDataPacket->data); // we need an actual time
                strcpy(loraDataPacket->data->name, OTHER_NAME);
                loraDataPacket->data->lat = 45.455631;
                loraDataPacket->data->lon = 54.084960;
                loraDataPacket->ttl = TTL;

#if DEBUG_MODE
                Serial.println();
#endif

                _timeOfLastSendedPacket = millis();
                return true;
        }

        return false;
}
