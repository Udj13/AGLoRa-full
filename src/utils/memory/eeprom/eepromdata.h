/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#ifndef EEPROMDATA_INCLUDED
#define EEPROMDATA_INCLUDED

#include "../../../settings/settings.h"

struct EEPROMDATA {
     unsigned char counter;
     DATA data;
     unsigned char crc;
  };

  #endif
