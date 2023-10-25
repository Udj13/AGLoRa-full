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
  void clearDataPacket(DATA * loraDataPacket);
  void updateName(DATA * loraDataPacket);
  void updateSensors(DATA * loraDataPacket);
  void printPackage(DATA * loraDataPacket);

//  void request(String request);

private:
};


#endif

