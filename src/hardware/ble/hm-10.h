#ifndef BLE_INCLUDED
#define BLE_INCLUDED

#include <Arduino.h>


// ====================== BLE HEADER ==========================

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