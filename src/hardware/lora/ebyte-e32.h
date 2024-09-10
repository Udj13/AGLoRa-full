/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/
#include "../../settings/settings.h"


#ifdef EBYTE_E32 


#ifndef LORA_INCLUDED
#define LORA_INCLUDED

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "loraData.h"
#include "../indication/indication.h"


// ======================= LORA SECTION =====================

/*
  _   _      _      ____    _____     _        ___    ____       _    
 | | | |    / \    |  _ \  |_   _|   | |      / _ \  |  _ \     / \   
 | | | |   / _ \   | |_) |   | |     | |     | | | | | |_) |   / _ \  
 | |_| |  / ___ \  |  _ <    | |     | |___  | |_| | |  _ <   / ___ \ 
  \___/  /_/   \_\ |_| \_\   |_|     |_____|  \___/  |_| \_\ /_/   \_\
                                                                      
*/

// install from Arduino IDE or PlatformIO
// Docs: https://github.com/xreef/EByte_LoRa_E220_Series_Library
#include "LoRa_E32.h"

class LORA
{
    SoftwareSerial loraPort;
    LoRa_E32 e32ttl;
public:
    LORA(uint8_t pinRx, uint8_t pinTx, uint8_t aux, uint8_t m0, uint8_t m1, INDICATION * indication);
    void setup();
    void send(LORADATA *loraDataPackage);
    bool hasNewData(LORADATA *loraDataPackage);


private:
    bool _debugMode;
    uint8_t _ledPin;
    INDICATION * _indication;
    ResponseStructContainer rsc;
};



#endif

#endif