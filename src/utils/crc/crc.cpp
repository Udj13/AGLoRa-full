#include <Arduino.h>
#include "crc.h"

// ======================== UTILITES ===============================

/*
          _     _   _   _   _                
  _   _  | |_  (_) | | (_) | |_    ___   ___ 
 | | | | | __| | | | | | | | __|  / _ \ / __|
 | |_| | | |_  | | | | | | | |_  |  __/ \__ \
  \__,_|  \__| |_| |_| |_|  \__|  \___| |___/
                                             
*/


// CRC
unsigned char calculateCRC(unsigned char *buffer, unsigned char size) {
  byte crc = 0;
  for (byte i = 0; i < size; i++) {
    byte data = buffer[i];
    for (int j = 8; j > 0; j--) {
      crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
      data >>= 1;
    }
  }
  return crc;
}