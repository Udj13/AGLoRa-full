#ifndef AGLORA_INCLUDED
#define AGLORA_INCLUDED

#include <Arduino.h>
#include "../settings/settings.h"
#include "../hardware/gps/gps.h"


class AGLORA
{
public:
  AGLORA();
  void hello();
  void updateSensors(DATA * loraDataPacket);
  bool updateLocation(GPS *gps, DATA * loraDataPacket);

//  void request(String request);

private:
  void printPackage(DATA * loraDataPacket);
};


#endif

