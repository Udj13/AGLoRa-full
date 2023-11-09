#ifndef AGLORA_INCLUDED
#define AGLORA_INCLUDED

#include "../settings/settings.h"
#include "../hardware/lora/loraData.h"
#include "../hardware/gps/gps.h"
#include "../hardware/ble/hm-10.h"
#include "../utils/memory/sram/sram.h"
#include "../utils/memory/eeprom/eepromaglora.h"

class AGLORA
{
public:
  AGLORA(IMemory * memory, BLE_HM10 * ble);
  void hello();
  void clearDataPacket(DATA * loraDataPacket);
//  void updateName(DATA * loraDataPacket);
  void updateSensors(DATA * loraDataPacket);
  void printPackage(LORADATA * loraDataPacket);
  void getRequest(String request);
  void sendPackageToBLE(DATA * loraDataPacket, int index);

private:
  IMemory * _memory;
  BLE_HM10 * _ble;
  void checkMemory();
  void sendAllPackagesToBLE();
  void sendPackageToBLEFromStorage(unsigned int index);

};


#endif

