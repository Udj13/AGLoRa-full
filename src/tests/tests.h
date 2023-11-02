#ifndef TESTS_INCLUDED
#define TESTS_INCLUDED

#include <Arduino.h>
#include "../settings/settings.h"
#include "../hardware/gps/gps.h"


#define OTHER_NAME "Morty" // virtual tracker's name

class TESTS
{
public:
    bool hasNewDataEveryXSec(DATA *loraDataPacket, GPS *gps, byte interval);

private:
    unsigned long _timeOfLastSendedPacket = 0;

};

#endif