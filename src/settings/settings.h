/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Modules used:
- Arduino UNO/Nano (ATMEGA328P, not ATmega168)
- GPS NMEA Module (Generic)
- LoRa EBYTE E220-900T22D (868 MHz) or EBYTE E32-E433T30D (433 MHz)
- Bluetooth BLE AT-09 or HC-05
*/

#ifndef SETTINGS_INCLUDED
#define SETTINGS_INCLUDED

/* 
HOW THIS SKETCH WORKS :

Most of the time the tracker works in listening mode. 

After receiving the LoRa package from another tracker - 
this data is transferred to the bluetooth serial port.

Once every three minutes, the tracker switches to 
the GPS receiver for a few seconds to get coordinates. 
Then sends a LoRa data packet.

NOTE: GPS is valid, if LED_BUILTIN is HIGH
*/

// HOW TO SETUP:
// ========================================
// ==== Settings LEVEL 1 (required) =======
// ========================================

// First of all, select the LORA module and the platform:
//#define ARDUINO_AVR_EBYTE_E220 // Arduino AVR + EBYTE_E220
//#define ARDUINO_AVR_EBYTE_E32 // Arduino AVR + EBYTE_E32
//#define ARDUINO_AVR_EBYTE_E22 // Arduino AVR + EBYTE_E22
//#define ESP32_C3_EBYTE_E32 // ESP32C3 + EBYTE_E32
#define ESP32_C3_EBYTE_E220 // ESP32C3 + EBYTE_E220
//#define ESP32_SX126X // SX126x (LILYGO TTGO T-Beam Devices)

/*  
Then, for the correct selection of the receiver сheck the selected PlatformIO config.
There are many options now:

1. ebyte-e220-nanoatmega328new
2. ebyte-e220-nanoatmega328old

3. ebyte-e32-nanoatmega328new
4. ebyte-e32-nanoatmega328old

5. ebyte-e22-nanoatmega328new
6. ebyte-e22-nanoatmega328old

7. ebyte-e220-esp32c3
8. ebyte-e220-esp32c3

9. ebyte-e32-esp32c3
10. ebyte-e32-esp32c3


The configuration connects the required library.
The libraries will be installed automatically.
*/

// Here are the libraries that I used in this project, thanks to their author Renzo Mischianti!
// Please, give him a donation if you also think he did a great job!
// https://github.com/xreef
// https://mischianti.org/
#ifdef ARDUINO_AVR_EBYTE_E32 
#include "LoRa_E32.h"      
// Docs: https://github.com/xreef/EByte_LoRa_E220_Series_Library
#endif
#ifdef ARDUINO_AVR_EBYTE_E22 
#include "LoRa_E22.h"
// Docs: https://github.com/xreef/EByte_LoRa_E22_Series_Library
#endif
//or
#ifdef ARDUINO_AVR_EBYTE_E220 
#include "LoRa_E220.h"
// Docs: https://github.com/xreef/LoRa_E32_Series_Library
#endif
//or
#ifdef ESP32_SX126X
#include "SX126x-Arduino.h"
#include <SPI.h>
// Docs: https://github.com/beegee-tokyo/SX126x-Arduino
#endif
//or
#ifdef ESP32_C3_EBYTE_E32
#include "LoRa_E32.h"      
// Docs: https://github.com/xreef/EByte_LoRa_E220_Series_Library
#endif
#ifdef ESP32_C3_EBYTE_E220
#include "LoRa_E220.h"
// Docs: https://github.com/xreef/LoRa_E32_Series_Library
#endif




// After selecting a module, you need to set up a tracker name and module connections:

// ========== NAME =======================
#define NAME_LENGTH 12             // The same value for all devices
const char NAME[NAME_LENGTH] = "Dipper";               // Name of current tracker, NAME_LENGTH characters
// Example:
// #define NAME = "Morty"; // All names length should be no longer than NAME_LENGTH
// ========== WIRING =====================
/* 
// ---------- Arduino pins example --------------
//UART LORA ARDUINO
#define LORA_PIN_RX 2
#define LORA_PIN_TX 3
#define LORA_PIN_M0 4
#define LORA_PIN_M1 5
#define LORA_PIN_AX 6

//UART GPS ARDUINO
#define GPS_PIN_RX 8 // RX GPS ARDUINO
#define GPS_PIN_TX 7 // TX GPS ARDUINO

// Leds
#define LORA_LED LED_BUILTIN
#define GPS_LED LED_BUILTIN
#define BLE_LED LED_BUILTIN
#define MEMORY_LED LED_BUILTIN
*/

/**/ 
// ---------- seeed xiao esp32c3 pins example --------------
//UART EBYTE LORA -> ESP32C3 SEED XIAO
#define LORA_PIN_RX 21  // GPIO20 or D7
#define LORA_PIN_TX 20  // GPIO21 or D6
#define LORA_PIN_M0 9   // GPIO9 or D9
#define LORA_PIN_M1 10  // GPIO10 or D10
#define LORA_PIN_AX 8   // GPIO8 or D8

//UART GPS ->  ESP32C3 SEED XIAO
#define GPS_PIN_RX 5  // GPIO4 or D2
#define GPS_PIN_TX 4  // GPIO5 or D3

// Leds
#define LORA_LED 2 // GPIO2 or D0
#define GPS_LED 2 // GPIO2 or D0
#define BLE_LED 2 // GPIO2 or D0
#define MEMORY_LED 2 // GPIO2 or D0
/**/


/*
// ---------- esp32-c3-devkitm-1 pins example --------------
//UART EBYTE LORA -> ESP32-C3 Super Mini Development Board
#define LORA_PIN_RX 3
#define LORA_PIN_TX 2
#define LORA_PIN_M0 0
#define LORA_PIN_M1 1
#define LORA_PIN_AX 4

//UART GPS ->  ESP32-C3 Super Mini Development Board
// #define GPS_PIN_RX 5  // GPIO4 or D2
// #define GPS_PIN_TX 4  // GPIO5 or D3
#define GPS_PIN_RX 21 // RX GPS ESP32
#define GPS_PIN_TX 20 // TX GPS ESP32

// Leds
#define LORA_LED 5 // GPIO2 or D0
#define GPS_LED 5 // GPIO2 or D0
#define BLE_LED 5 // GPIO2 or D0
#define MEMORY_LED 5 // GPIO2 or D0
*/




// ========== MODULES SETTING=============
#define GPS_SPEED 9600
#define USE_HARDWARE_GPS_UART false

#define LORA_START_SPEED 9600

// Then install "EByte LoRa E220 by Renzo Mischianty" library
// and "TinyGPSPlus by Mikal Hart" library
// from Arduino IDE library manager ("Tools" -> "Manage Libraries")

// Now you can upload this sketch to Arduino
// and turn on the app "AGLoRa" on your phone

// If something went wrong you can enable
// "debug mode" through the serial port.
// Don't forget to disconnect the bluetooth module.
// Then open “Tools” -> ”Serial monitor” in Arduino IDE.
#define DEBUG_MODE true  // change "false" to "true" to enable
// Next, logs levels for comfortable deallbugging, 
// if DEBUG_MODE == false, logs level are not important 
#define DEBUG_BLE true  // bluetooth low energy

#define DEBUG_GPS true  // print GPS logs
#define DEBUG_PRINT_FULL_GPS_LOG true  // print full GPS NMEA

#define DEBUG_LORA true  // print GPS logs
#define DEBUG_MEMORY true  // print GPS logs
#define DEBUG_AGLORA true  // print GPS logs

// ========================================
// ==== Settings LEVEL 2 (optional) =======
// ========================================

/*
This is the structure of the LoRa data package.
If you want to send additional data between AGLoRa trackers
you should add it to this section.
Note that all software version must be the same for all trackers.
*/
#pragma pack(push, 1) 
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

  // Add more data fields here if you need
  // ...
  unsigned char sats;
  //unsigned char hdop;
  float altitude;
  unsigned char battery;
  // unsigned char speed;
  // unsigned char sensor1;
  // unsigned char sensor2;
  // ...
};
#pragma pack(pop)

/*
This is a function that sends data to the app.
Data packets are sent using OsmAnd-like protocol:
id=name&lat={0}&lon={1}&timestamp={2}&speed={3}&altitude={4}
*/
String sendToPhone(DATA *package); 
String sendBatteryToPhone(); 
// find it in the code and customize if you need

// ========================================
// ==== Settings LEVEL 3 (nightmare) ======
// ========================================
#define USE_EEPROM_MEMORY false  // "false" by default
// set "false" to use SRAM memory, "true" to use EEPROM
// EEPROM is permanent memory, data is not lost even 
// if the system is turned off.
// But the write operation is finite and usually capped at 100,000 cycles.
// Please read: https://docs.arduino.cc/learn/programming/memory-guide
// ============ LORA NETWORK SETTINGS ============
#define I_WANT_TO_SEND_MY_LOCATION true  // "true" by default
#define DATA_SENDING_INTERVAL 30000  // milliseconds (seconds * 1000)

#define MESH_MODE true  // "true" by default
#define TTL 3  // Data packet lifetime (for transfer between devices)
// ============ BLE SETTINGS ==========
#define USE_BLE true  // use BLE output
#define USE_BLE_MTU true // split data by MTU
#define BLE_UPDATE_INTERVAL 50000  // milliseconds (seconds * 1000)
// ============ SRAM STORAGE ==============
// Maximum number of track points (struct DATA) in memory
// Change and check free memory in "Output" after pressing "Verify".
#define SRAM_STORAGE_SIZE 15    // DATA array size
// not used if USE_EEPROM_MEMORY true, may be zero in this case
// NOTE: Don't use all free memory! It's may broke BLE output. 
// You should hold free memory for return String from "sendToPhone"
//
// ⚠️ If you are using Arduino, set this parameter SRAM_STORAGE_SIZE  to 10 or 15 and
//    carefully increase it!
// ⚠️ Note that enabling DEBUG_MODE significantly increases memory usage.
// ============ EEPROM STORAGE ==============
// EEPROM (non-volatile) memory
// not used if define USE_EEPROM_MEMORY false
#define EEPROM_BEGIN_ADDRESS 0  //bytes
// reserve for storing settings
// not used if USE_EEPROM_MEMORY false
// ================ TESTS ==================
#define TEST_LORA_DATA false
// =========================================
// ========== END OF SETTINGS ==============
// =========================================

#endif

