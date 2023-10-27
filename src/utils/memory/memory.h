#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED


#include <Arduino.h>
#include "../../settings/settings.h"


class IMemory {
    public:
        virtual void setup() = 0;
        virtual void clearAllPositions() = 0;
        virtual bool checkUnique(DATA *loraDataPacket) = 0;
        virtual void save(DATA *loraDataPacket) = 0;
};


#endif