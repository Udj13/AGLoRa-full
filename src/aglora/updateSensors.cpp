#include "aglora.h"

/// Udpate sensors in loraDataPacket
void AGLORA::updateSensors(DATA *loraDataPacket)
{
    loraDataPacket->battery = 100; // just for example
}
