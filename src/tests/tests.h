/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

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