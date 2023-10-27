#ifndef TESTS_INCLUDED
#define TESTS_INCLUDED

#include <Arduino.h>
#include "../settings/settings.h"
#include "../hardware/gps/gps.h"


#define OTHER_NAME "Morty" // virtual tracker's name
#define TEST_DATA_INTERVAL 60000 // test data interval in ms

class TESTS
{
public:
    bool hasNewDataEvery60sec(DATA *loraDataPacket, GPS *gps);

private:
    unsigned long _timeOfLastSendedPacket = 0;

};

#endif