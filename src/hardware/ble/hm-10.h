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


class BLE_HM10
{
public:
    BLE_HM10();
    void setup();
    String read();
    void send(String * package);

private:
    const byte MTU = 22;
    void sendCommand(const String command);
};

#endif