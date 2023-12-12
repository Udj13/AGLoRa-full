/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#ifndef AGLORA_INCLUDED
#define AGLORA_INCLUDED

#include "../settings/settings.h"
#include "../hardware/lora/loraData.h"
#include "../hardware/gps/gps.h"
#include "../hardware/ble/hm-10.h"
#include "../utils/memory/sram/sram.h"
#include "../utils/memory/eeprom/eepromaglora.h"


/*****************************************************
     _       ____   _        ___    ____       _    
    / \     / ___| | |      / _ \  |  _ \     / \   
   / _ \   | |  _  | |     | | | | | |_) |   / _ \  
  / ___ \  | |_| | | |___  | |_| | |  _ <   / ___ \ 
 /_/   \_\  \____| |_____|  \___/  |_| \_\ /_/   \_\
******************************************************/


class AGLORA
{
public:
  AGLORA(IMemory * memory, BLE_HM10 * ble);
  void hello();
  void checkMemoryToBLE();
  void clearDataPacket(DATA * trackerData);
  void updateSensors(DATA * trackerData);
  void printPackage(LORADATA * loraDataPacket);
  void getRequest(String request);
  void sendPackageToBLE(DATA * trackerData, int index);

private:
  IMemory * _memory;
  BLE_HM10 * _ble;
  void sendAllPackagesToBLE();
  void sendPackageToBLEFromStorage(unsigned int index);

};


#endif

