#include <Arduino.h>


// ====================== BLE HEADER ==========================

class BLE
{
public:
    BLE(bool debugMode);
    void setup();
    void read(void storageManager(String *command));

private:
    void sendCommand(const String command);
    bool _debugMode;
};
