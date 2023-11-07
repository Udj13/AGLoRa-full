#ifndef IMEMORY_INCLUDED
#define IMEMORY_INCLUDED


#include <Arduino.h>
#include "../../settings/settings.h"


class IMemory {         // interface
    public:
        virtual void setup() = 0;

        virtual void clearAllPositions() = 0;
        virtual bool checkUnique(DATA *loraDataPacket) = 0;
        virtual unsigned int save(DATA *loraDataPacket) = 0;
        virtual DATA * load(unsigned int index) = 0;

        virtual bool checkCRC() = 0;    // all memory
        virtual bool checkCRC(DATA *loraDataPacket) = 0;
        virtual bool checkCRC(unsigned int index) = 0;

        virtual unsigned int getSize() = 0;
        virtual unsigned int getIndex() = 0;
        virtual bool getStorageOverwrite() = 0;
};


#endif