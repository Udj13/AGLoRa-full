/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#if defined(ESP32_C3_EBYTE_E32) || defined(ESP32_C3_EBYTE_E220) || defined(ESP32_SX126X)

#ifndef BLE_INCLUDED
#define BLE_INCLUDED

#include <Arduino.h>


// ========================== BLE SECTION ==============================
/*
  ____    _       _   _   _____   _____    ___     ___    _____   _   _ 
 | __ )  | |     | | | | | ____| |_   _|  / _ \   / _ \  |_   _| | | | |
 |  _ \  | |     | | | | |  _|     | |   | | | | | | | |   | |   | |_| |
 | |_) | | |___  | |_| | | |___    | |   | |_| | | |_| |   | |   |  _  |
 |____/  |_____|  \___/  |_____|   |_|    \___/   \___/    |_|   |_| |_|
                                                                        
*/


class BLE
{
public:
    BLE();
    void setup();
    String read();
    void send(String * package);

private:
    const byte MTU = 20;
    void sendWithMTU(String * package);
    void sendWithoutMTU(String * package);
};

#endif
#endif