#ifndef AGLORA_INCLUDED
#define AGLORA_INCLUDED

#include <Arduino.h>
#include "../settings/settings.h"


class AGLORA
{
public:
  AGLORA();
  void hello();
  void updateSensors(DATA * loraDataPacket);
//  void updateLocation(GPS *gps, DATA * loraDataPacket);

//  void request(String request);
};


#endif

