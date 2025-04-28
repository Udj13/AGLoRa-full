/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#ifndef IMEMORY_INCLUDED
#define IMEMORY_INCLUDED


#include <Arduino.h>
#include "../../settings/settings.h"



/*
                                                      
  _ __ ___     ___   _ __ ___     ___    _ __   _   _ 
 | '_ ` _ \   / _ \ | '_ ` _ \   / _ \  | '__| | | | |
 | | | | | | |  __/ | | | | | | | (_) | | |    | |_| |
 |_| |_| |_|  \___| |_| |_| |_|  \___/  |_|     \__, |
                                                |___/ 
*/


class IMemory {         // interface
    public:
        virtual void setup() = 0;

        virtual void clearAllPositions() = 0;
        virtual bool checkUnique(DATA *newPoint) = 0;
        virtual unsigned int save(DATA *newData) = 0;
        virtual DATA * load(unsigned int index) = 0;

        virtual bool checkCRC() = 0;    // all memory
        virtual bool checkCRC(unsigned int index) = 0;

        virtual unsigned int getSize() = 0;
        virtual unsigned int getIndex() = 0;
        virtual bool getStorageOverwrite() = 0;
};


#endif