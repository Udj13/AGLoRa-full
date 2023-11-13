#ifndef TESTS_INCLUDED
#define TESTS_INCLUDED

#include <Arduino.h>
#include "../settings/settings.h"
#include "../hardware/lora/loraData.h"
#include "../hardware/gps/gps.h"


// ================= TESTS SECTION =================
/*
 _____   _____   ____    _____   ____  
 |_   _| | ____| / ___|  |_   _| / ___| 
   | |   |  _|   \___ \    | |   \___ \ 
   | |   | |___   ___) |   | |    ___) |
   |_|   |_____| |____/    |_|   |____/ 
                                        
*/


#define OTHER_NAME "Morty" // virtual tracker's name

class TESTS
{
public:
    bool hasNewDataEveryXSec(LORADATA *loraDataPacket, GPS *gps, byte interval);

private:
    unsigned long _timeOfLastSendedPacket = 0;

};

#endif