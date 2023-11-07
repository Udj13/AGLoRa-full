#ifndef EEPROMDATA_INCLUDED
#define EEPROMDATA_INCLUDED

#include "../../../settings/settings.h"

struct EEPROMDATA {
     unsigned char counter;
     DATA data;
     unsigned char crc;
  };

  #endif
