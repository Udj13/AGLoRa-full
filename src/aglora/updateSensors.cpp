/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#include "aglora.h"

/// Udpate sensors in loraDataPacket
void AGLORA::updateSensors(DATA *loraDataPacket)
{
    loraDataPacket->battery = 100; // just for example

#if DEBUG_MODE && DEBUG_AGLORA
    Serial.print(F("🟢[AGLoRa sensors: "));
    Serial.print(F("🔋 - "));
    Serial.print(loraDataPacket->battery);
    Serial.println(F("]"));
#endif
}
