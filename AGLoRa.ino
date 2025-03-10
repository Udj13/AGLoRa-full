/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty.
*/

// ========== SETTINGS SECTION ==========
// First, select EBYTE module:
#define EBYTE_E220 // EBYTE_E220 or EBYTE_E32
//#define EBYTE_E32 // EBYTE_E220 or EBYTE_E32

// Required libraries:
// 1. EByte LoRa E220/E32 by Renzo Mischianti
// 2. TinyGPSPlus by Mikal Hart

// ========== NAME =======================
#define NAME_LENGTH 12             // The same value for all devices
const char NAME[NAME_LENGTH] = "Morty";               // Name of current tracker, NAME_LENGTH characters

// ========== WIRING =====================
//UART LORA
#define LORA_PIN_RX 2
#define LORA_PIN_TX 3
#define LORA_PIN_M0 4
#define LORA_PIN_M1 5
#define LORA_PIN_AX 6

//UART GPS
#define GPS_PIN_RX 7
#define GPS_PIN_TX 8

// Leds
#define LORA_LED LED_BUILTIN
#define GPS_LED LED_BUILTIN
#define BLE_LED LED_BUILTIN
#define MEMORY_LED LED_BUILTIN

// ========== MODULES SETTING=============
#define GPS_SPEED 9600
#define LORA_START_SPEED 9600

// Debug settings
#define DEBUG_MODE false  // change "false" to "true" to enable
#define DEBUG_BLE false  // bluetooth low energy
#define DEBUG_GPS true  // print GPS logs
#define DEBUG_LORA true  // print LoRa logs
#define DEBUG_MEMORY true  // print memory logs
#define DEBUG_AGLORA true  // print AGLoRa logs

// ========== MEMORY SETTINGS ============
#define USE_EEPROM_MEMORY false  // "false" by default
#define SRAM_STORAGE_SIZE 15    // DATA array size
#define EEPROM_BEGIN_ADDRESS 0  //bytes

// ========== NETWORK SETTINGS ============
#define I_WANT_TO_SEND_MY_LOCATION true  // "true" by default
#define DATA_SENDING_INTERVAL 30000  // milliseconds (seconds * 1000)
#define MESH_MODE true  // "true" by default
#define TTL 3  // Data packet lifetime (for transfer between devices)

// ========== BLE SETTINGS ============
#define USE_BLE true  // use BLE output
#define BLE_UPDATE_INTERVAL 50000  // milliseconds (seconds * 1000)
#define MTU 20  // Maximum Transmission Unit

// ========== TEST SETTINGS ============
#define TEST_LORA_DATA false

// ========== DATA STRUCTURES ============
struct DATA {
  char name[NAME_LENGTH];  // name
  float lat;  // coordinates
  float lon;
  unsigned char year;  // the Year minus 2000
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
  bool gpsValid;
  unsigned char battery;
};

struct LORADATA {
    DATA data;
    unsigned char ttl;   // time to live (for mesh network)
};

// ========== INCLUDES ============
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <EEPROM.h>

#ifdef EBYTE_E32 
#include "LoRa_E32.h"
#endif

#ifdef EBYTE_E220 
#include "LoRa_E220.h"
#endif

// ========== CLASS DEFINITIONS ============

// Indication class
enum class GPSStatuses { correct, invalid, connectionError };
enum class LoRaStatuses { dataReceived, dataTransmitted, error };
enum class BLEStatuses { output, input, error };
enum class MemoryStatuses { read, write, crcError };

class INDICATION {
public:
    INDICATION(uint8_t gpsLedPin, uint8_t loraLedPin, uint8_t bleLedPin, uint8_t memoryLedPin) {
        _gpsLedPin = gpsLedPin;
        _loraLedPin = loraLedPin;
        _bleLedPin = bleLedPin;
        _memoryLedPin = memoryLedPin;
        
        pinMode(_gpsLedPin, OUTPUT);
        pinMode(_loraLedPin, OUTPUT);
        pinMode(_bleLedPin, OUTPUT);
        pinMode(_memoryLedPin, OUTPUT);
    }

    void gps(GPSStatuses status) {
        switch(status) {
            case GPSStatuses::correct:
                digitalWrite(_gpsLedPin, HIGH);
                break;
            case GPSStatuses::invalid:
                digitalWrite(_gpsLedPin, LOW);
                break;
            case GPSStatuses::connectionError:
                digitalWrite(_gpsLedPin, LOW);
                break;
        }
    }

    void lora(LoRaStatuses status) {
        switch(status) {
            case LoRaStatuses::dataReceived:
                digitalWrite(_loraLedPin, HIGH);
                lastLoraUpdateTime = millis();
                break;
            case LoRaStatuses::dataTransmitted:
                digitalWrite(_loraLedPin, LOW);
                break;
            case LoRaStatuses::error:
                digitalWrite(_loraLedPin, LOW);
                break;
        }
    }

    void ble(BLEStatuses status) {
        switch(status) {
            case BLEStatuses::output:
                digitalWrite(_bleLedPin, HIGH);
                break;
            case BLEStatuses::input:
                digitalWrite(_bleLedPin, LOW);
                break;
            case BLEStatuses::error:
                digitalWrite(_bleLedPin, LOW);
                break;
        }
    }

    void memory(MemoryStatuses status) {
        switch(status) {
            case MemoryStatuses::read:
                digitalWrite(_memoryLedPin, HIGH);
                break;
            case MemoryStatuses::write:
                digitalWrite(_memoryLedPin, LOW);
                break;
            case MemoryStatuses::crcError:
                digitalWrite(_memoryLedPin, LOW);
                break;
        }
    }

    void loop() {
        if ((millis() - lastLoraUpdateTime) > (loraDelaySec * 1000)) {
            digitalWrite(_loraLedPin, LOW);
        }
    }

private:
    uint8_t _gpsLedPin;
    uint8_t _loraLedPin;
    uint8_t _bleLedPin;
    uint8_t _memoryLedPin;
    bool loraLedStatus = false;
    const byte loraDelaySec = 1;
    unsigned long lastLoraUpdateTime;
};

// GPS class
class GPS {
    SoftwareSerial gpsPort;
    TinyGPSPlus gps;
    INDICATION * _indication;
    char _readingIndicator = 0;

public:
    GPS(uint8_t pinRx, uint8_t pinTx, long speed, INDICATION * indication) 
        : gpsPort(pinRx, pinTx), _indication(indication) {
        gpsPort.begin(speed);
    }

    void setup() {
        _indication->gps(GPSStatuses::invalid);
    }

    void updateLocation(DATA *dataPackage) {
        unsigned long start = millis();
        bool gpsValid = false;
        
        while ((millis() - start) < 5000) { // 5 second timeout
            if (gpsPort.available()) {
                char c = gpsPort.read();
                if (gps.encode(c)) {
                    if (gps.location.isValid()) {
                        dataPackage->lat = gps.location.lat();
                        dataPackage->lon = gps.location.lng();
                        dataPackage->year = gps.date.year() - 2000;
                        dataPackage->month = gps.date.month();
                        dataPackage->day = gps.date.day();
                        dataPackage->hour = gps.time.hour();
                        dataPackage->minute = gps.time.minute();
                        dataPackage->second = gps.time.second();
                        dataPackage->gpsValid = true;
                        gpsValid = true;
                        _indication->gps(GPSStatuses::correct);
                        break;
                    }
                }
            }
        }
        
        if (!gpsValid) {
            dataPackage->gpsValid = false;
            _indication->gps(GPSStatuses::invalid);
        }
    }
};

// LoRa class
class LORA {
    SoftwareSerial loraPort;
    #ifdef EBYTE_E32
    LoRa_E32 e32ttl;
    #endif
    #ifdef EBYTE_E220
    LoRa_E220 e220ttl;
    #endif
    INDICATION * _indication;

public:
    LORA(uint8_t pinRx, uint8_t pinTx, uint8_t aux, uint8_t m0, uint8_t m1, INDICATION * indication) 
        : loraPort(pinRx, pinTx), _indication(indication) {
        #ifdef EBYTE_E32
        e32ttl = LoRa_E32(aux, m0, m1);
        #endif
        #ifdef EBYTE_E220
        e220ttl = LoRa_E220(aux, m0, m1);
        #endif
    }

    void setup() {
        #ifdef EBYTE_E32
        e32ttl.begin();
        e32ttl.resetModule();
        #endif
        #ifdef EBYTE_E220
        e220ttl.begin();
        e220ttl.resetModule();
        #endif
        
        loraPort.begin(LORA_START_SPEED);
        _indication->lora(LoRaStatuses::dataTransmitted);
    }

    void send(LORADATA *loraDataPackage) {
        loraPort.listen();
        const byte LORADATASIZE = sizeof(LORADATA);
        
        #ifdef EBYTE_E32
        ResponseStatus rs = e32ttl.sendMessage(loraDataPackage, LORADATASIZE);
        #endif
        #ifdef EBYTE_E220
        ResponseStatus rs = e220ttl.sendMessage(loraDataPackage, LORADATASIZE);
        #endif
        
        if (rs.code == 1) {
            _indication->lora(LoRaStatuses::dataTransmitted);
        } else {
            _indication->lora(LoRaStatuses::error);
        }
    }

    bool hasNewData(LORADATA *loraDataPackage) {
        if (loraPort.available()) {
            loraPort.listen();
            if (loraPort.read() == 0) {
                if (loraPort.read() == 0) {
                    if (loraPort.read() == sizeof(LORADATA)) {
                        loraPort.readBytes((uint8_t*)loraDataPackage, sizeof(LORADATA));
                        _indication->lora(LoRaStatuses::dataReceived);
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

// BLE class
class BLE_HM10 {
public:
    BLE_HM10() {}

    void setup() {
        #if DEBUG_MODE && DEBUG_BLE
        Serial.print(F("📲[BLE: ready for work ✅. Maximum Transmission Unit (MTU) = "));
        Serial.print(MTU);
        Serial.println(F("]"));
        #endif
        #if !DEBUG_MODE
        sendCommand(F("AT"));
        sendCommand(F("AT+NAMEAGLoRa"));
        sendCommand(F("AT+ROLE0"));
        #endif
    }

    String read() {
        String result = "";
        while (Serial.available()) {
            result += Serial.readString();
        }
        result.trim();
        return result;
    }

    void send(String *package) {
        bool isStringNotEmpty = true;
        while (isStringNotEmpty) {
            String nextSendMTU = package->substring(0, MTU);
            package->remove(0, MTU);
            isStringNotEmpty = package->length() != 0;
            #if !DEBUG_MODE && !DEBUG_BLE
            Serial.print(nextSendMTU);
            delay(10);
            #endif
        }
    }

private:
    void sendCommand(const char* command) {
        Serial.println(command);
        delay(100);
    }
};

// Memory interface
class IMemory {
public:
    virtual void setup() = 0;
    virtual bool checkUnique(DATA *newPoint) = 0;
    virtual unsigned int save(DATA *newData) = 0;
    virtual DATA * load(unsigned int index) = 0;
    virtual bool checkCRC() = 0;
    virtual bool checkCRC(unsigned int index) = 0;
    virtual unsigned int getSize() = 0;
    virtual unsigned int getIndex() = 0;
    virtual bool getStorageOverwrite() = 0;
};

// SRAM implementation
class SRAM : public IMemory {
    DATA *storage;
    unsigned int storageIndex = 0;
    bool storageOverwrite = false;
    unsigned int dataSize;

public:
    SRAM() {
        dataSize = sizeof(DATA);
        storage = new DATA[SRAM_STORAGE_SIZE + 1];
    }

    void setup() {
        #if DEBUG_MODE && DEBUG_MEMORY
        Serial.print(F("💾[SRAM storage: memory is ready. SRAM_STORAGE_SIZE="));
        Serial.print(SRAM_STORAGE_SIZE + 1);
        Serial.print(F(" ("));
        Serial.print((SRAM_STORAGE_SIZE + 1) * sizeof(DATA));
        Serial.print(F(" bytes)"));
        Serial.println(F("]"));
        #endif
    }

    bool checkUnique(DATA *newPoint) {
        for (unsigned int i = 0; i < storageIndex; i++) {
            if (strcmp(storage[i].name, newPoint->name) == 0 &&
                storage[i].year == newPoint->year &&
                storage[i].month == newPoint->month &&
                storage[i].day == newPoint->day &&
                storage[i].hour == newPoint->hour &&
                storage[i].minute == newPoint->minute) {
                return false;
            }
        }
        return true;
    }

    unsigned int save(DATA *newData) {
        if (storageIndex >= SRAM_STORAGE_SIZE) {
            storageOverwrite = true;
            storageIndex = 0;
        }
        memcpy(&storage[storageIndex], newData, dataSize);
        return storageIndex++;
    }

    DATA * load(unsigned int index) {
        if (index < storageIndex) {
            return &storage[index];
        }
        return nullptr;
    }

    bool checkCRC() {
        return true; // Implement if needed
    }

    bool checkCRC(unsigned int index) {
        return true; // Implement if needed
    }

    unsigned int getSize() {
        return SRAM_STORAGE_SIZE;
    }

    unsigned int getIndex() {
        return storageIndex;
    }

    bool getStorageOverwrite() {
        return storageOverwrite;
    }
};

// AGLoRa main class
class AGLORA {
    IMemory * _memory;
    BLE_HM10 * _ble;

public:
    AGLORA(IMemory * memory, BLE_HM10 * ble) : _memory(memory), _ble(ble) {}

    void hello() {
        Serial.println(F("\n\n"));
        Serial.println(F("   _    ____    _      "));
        Serial.println(F("  / \\  | __ )  | |     "));
        Serial.println(F(" / _ \\ |  _ \\  | |     "));
        Serial.println(F("/ ___ \\| |_) | | |___  "));
        Serial.println(F("/_/   \\_\\____/ |_____| "));
        Serial.println(F("                        "));
        Serial.println(F("AGLoRa GPS Tracker"));
        Serial.println(F("https://github.com/Udj13/AGLoRa/"));
        Serial.println(F("\n"));
    }

    void clearDataPacket(DATA * trackerData) {
        memset(trackerData, 0, sizeof(DATA));
        strcpy(trackerData->name, NAME);
    }

    void updateSensors(DATA * trackerData) {
        trackerData->battery = 100; // Implement actual battery reading
    }

    void printPackage(LORADATA * loraDataPacket) {
        #if DEBUG_MODE && DEBUG_AGLORA
        Serial.print(F("📦[AGLoRa: Package from "));
        Serial.print(loraDataPacket->data.name);
        Serial.print(F(" "));
        Serial.print(loraDataPacket->data.lat, 6);
        Serial.print(F(" "));
        Serial.print(loraDataPacket->data.lon, 6);
        Serial.print(F(" TTL="));
        Serial.print(loraDataPacket->ttl);
        Serial.println(F("]"));
        #endif
    }

    void getRequest(String request) {
        if (request.length() > 0) {
            if (request.indexOf("GET") >= 0) {
                sendAllPackagesToBLE();
            }
        }
    }

    void sendPackageToBLE(DATA * trackerData, int index) {
        String package = sendToPhone(trackerData);
        _ble->send(&package);
    }

private:
    void sendAllPackagesToBLE() {
        for (unsigned int i = 0; i < _memory->getIndex(); i++) {
            sendPackageToBLEFromStorage(i);
        }
    }

    void sendPackageToBLEFromStorage(unsigned int index) {
        DATA * trackerData = _memory->load(index);
        if (trackerData != nullptr) {
            sendPackageToBLE(trackerData, index);
        }
    }
};

// Function to format data for phone
String sendToPhone(DATA *package) {
    String result;
    result += F("&dev_batt=");
    result += F("100");
    result += F("&name=");
    result += package->name;
    result += F("&lat=");
    result += String(package->lat, 6);
    result += F("&lon=");
    result += String(package->lon, 6);
    result += F("&timestamp=");
    result += package->year + 2000;
    result += F("-");
    if (package->month < 10) result += F("0");
    result += package->month;
    result += F("-");
    if (package->day < 10) result += F("0");
    result += package->day;
    result += F("T");
    if (package->hour < 10) result += F("0");
    result += package->hour;
    result += F(":");
    if (package->minute < 10) result += F("0");
    result += package->minute;
    result += F(":");
    if (package->second < 10) result += F("0");
    result += package->second;
    result += F("Z");
    result += F("&gpsValid=");
    result += package->gpsValid;
    result += "&batt=";
    result += package->battery;
    return result;
}

// ========== GLOBAL OBJECTS ============
INDICATION indication(GPS_LED, LORA_LED, BLE_LED, MEMORY_LED);
GPS gps(GPS_PIN_RX, GPS_PIN_TX, GPS_SPEED, &indication);
LORA lora(LORA_PIN_RX, LORA_PIN_TX, LORA_PIN_AX, LORA_PIN_M0, LORA_PIN_M1, &indication);
BLE_HM10 ble;

#if USE_EEPROM_MEMORY
// EEPROM implementation would go here
#else
SRAM memory;
#endif

LORADATA loraDataPackage;
AGLORA aglora(&memory, &ble);

// ========== MAIN FUNCTIONS ============
void setup() {
    Serial.begin(9600);
    aglora.hello();
    gps.setup();
    lora.setup();
    memory.setup();
    ble.setup();
}

unsigned long _timeToSendMyLocation = millis() + DATA_SENDING_INTERVAL;
unsigned long _timeOfLastReceivedPacket;
unsigned int addedMemoryIndex;
byte ttl = 0;

void processNewData(LORADATA *loraDataPackage) {
    if (memory.checkUnique(&loraDataPackage->data)) {
        ttl = loraDataPackage->ttl;
        addedMemoryIndex = memory.save(&loraDataPackage->data);
        memory.checkCRC();

        #if USE_BLE
        aglora.sendPackageToBLE(&loraDataPackage->data, addedMemoryIndex);
        #endif

        #if MESH_MODE
        if (--ttl > 0) {
            loraDataPackage->ttl = ttl;
            lora.send(loraDataPackage);
        }
        #endif
    }
    _timeOfLastReceivedPacket = millis();
}

void loop() {
    if (_timeToSendMyLocation < millis()) {
        #if I_WANT_TO_SEND_MY_LOCATION
        aglora.clearDataPacket(&loraDataPackage.data);
        aglora.updateSensors(&loraDataPackage.data);
        gps.updateLocation(&loraDataPackage.data);
        loraDataPackage.ttl = TTL;

        aglora.printPackage(&loraDataPackage);
        lora.send(&loraDataPackage);
        #endif

        _timeToSendMyLocation += DATA_SENDING_INTERVAL;
    }

    if (lora.hasNewData(&loraDataPackage)) {
        processNewData(&loraDataPackage);
    }

    if ((_timeOfLastReceivedPacket + BLE_UPDATE_INTERVAL) < millis()) {
        aglora.checkMemoryToBLE();
        _timeOfLastReceivedPacket = millis();
    }

    aglora.getRequest(ble.read());
    indication.loop();
} 