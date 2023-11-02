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
    void send(String command);

private:
    void sendCommand(const String command);
};

#endif