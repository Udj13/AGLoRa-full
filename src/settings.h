/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker
Copyright 2023 Eugeny Shlyagin (shlyagin@gmail.com)

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Modules used:
- Arduino UNO/Nano (ATMEGA328P, not ATmega168)
- GPS NMEA Module (Generic)
- LoRa EBYTE E220-900T22D (868 MHz) or EBYTE E32-E433T30D (433 MHz)
- Bluetooth BLE AT-09 or HC-05
*/

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

#include <TinyGPSPlus.h>      // install from Arduino IDE
// Docs: http://arduiniana.org/libraries/tinygpsplus/
#include "LoRa_E220.h"        // install from Arduino IDE
// Docs: https://github.com/xreef/EByte_LoRa_E220_Series_Library


// The first thing you need is to set up a tracker name and modules connections:

// ========== NAME =======================
#define NAME_LENGTH 6                // The same value for all devices
char MY_NAME[NAME_LENGTH] = "Rick";  // Name of current tracker, NAME_LENGTH characters
// Example:
// char MY_NAME[NAME_LENGTH] = "Morty"; // All names length should be no longer than NAME_LENGTH
// ========== WIRING =====================
#define LORA_PIN_RX 2
#define LORA_PIN_TX 3
#define LORA_PIN_M0 4
#define LORA_PIN_M1 5
#define LORA_PIN_AX 6

#define GPS_PIN_RX 7
#define GPS_PIN_TX 8

// Then install "EByte LoRa E220 by Renzo Mischianty" library
// and "TinyGPSPlus by Mikal Hart" library
// from Arduino IDE library manager ("Tools" -> "Manage Libraries")

// Now you can upload this sketch to Arduino
// and turn on the app "AGLoRa" on your phone

// If something went wrong you can enable
// "debug mode" through the serial port.
// Don't forget to disconnect the bluetooth module.
// Then open “Tools” -> ”Serial monitor” in Arduino IDE.
#define DEBUG_MODE false  // change "false" to "true" to enable

// ========================================
// ==== Settings LEVEL 2 (optional) =======
// ========================================

/*
This is the structure of the LoRa data package.
If you want to send additional data between AGLoRa trackers
you should add it to this section.
Note that all software version must be the same for all trackers.
*/
struct DATA {
  char id[NAME_LENGTH];  // name

  float lat;  // coordinates
  float lon;

  unsigned char year;  // the Year minus 2000
  unsigned char month;
  unsigned char day;

  unsigned char hour;
  unsigned char minute;
  unsigned char second;

  unsigned char ttl; // time to live (mesh network)

  unsigned char sat;

  // Add more data fields here if you need
  // ...
  // unsigned char speed;
  // unsigned char battery;
  // unsigned char sensor1;
  // ...
};



/*
This is a function that sends data to the app.
Data packets are sent using OsmAnd-like protocol:
id=name&lat={0}&lon={1}&timestamp={2}&speed={3}&altitude={4}
*/
void sendPackageToBluetooth(DATA *package) {

  Serial.print(F("&name="));  //other tracker's name
  Serial.print(package->id);  //NAME_LENGTH bytes

  Serial.print(F("&lat="));       // cordinates
  Serial.print(package->lat, 6);  // latitude
  Serial.print(F("&lon="));       // record separator
  Serial.print(package->lon, 6);  // longitute

  //Date and time format: 2023-06-07T15:21:00Z
  Serial.print(F("&timestamp="));      // record separator
  Serial.print(package->year + 2000);  // year
  Serial.print(F("-"));                // data separator
  if (package->month < 10) Serial.print(F("0"));
  Serial.print(package->month);        // month
  Serial.print(F("-"));                // data separator
  if (package->day < 10) Serial.print(F("0"));
  Serial.print(package->day);          // day
  Serial.print(F("T"));                // record separator
  if (package->hour < 10) Serial.print(F("0"));
  Serial.print(package->hour);         // hour
  Serial.print(F(":"));                // time separator
  if (package->minute < 10) Serial.print(F("0"));
  Serial.print(package->minute);       // minute
  Serial.print(F(":"));                // time separator
  if (package->second < 10) Serial.print(F("0"));
  Serial.print(package->second);       // second
  Serial.print(F("Z"));                // UTC

  // Sensors and additional data
  Serial.print(F("&sat="));    //record separator
  Serial.print(package->sat);  // satellites  1 byte

  // Add more data here if you need ...
  // Serial.print("&speed=");       // data's name in app
  // Serial.print(package->speed);   // value

  // Serial.print("&batt=");
  // Serial.print(package->battery);

  // Serial.print("&alienSensor=");
  // Serial.print(package->sensor1);

  Serial.println();   // record separator "\r\n" 
                      //and end of transmission
}


// ========================================
// ==== Settings LEVEL 3 (nightmare) ======
// ========================================
#define USE_EEPROM_MEMORY false  // "false" by default
// set "false" to use SRAM memory, "true" to use EEPROM
// EEPROM is permanent memory, data is not lost even 
// if the system is turned off.
// But the write operation is finite and usually capped at 100,000 cycles.
// Please read: https://docs.arduino.cc/learn/programming/memory-guide
// ============ GPS SETTINGS ============
#define DATA_PACKET_INTERVAL 20000  // milliseconds
// ============ OTHER SETTINGS ==========
#define BLE_UPDATE_INTERVAL 30000  // milliseconds
// ============ SRAM STORAGE ==============
// Maximum number of track points (struct DATA) in memory
// Change and check free memory in "Output" after pressing "Verify".
#define SRAM_STORAGE_SIZE 40    // DATA array size
// not used if USE_EEPROM_MEMORY true, may be zero in this case
// ============ EEPROM STORAGE ==============
// EEPROM (non-volatile) memory
// not used if define USE_EEPROM_MEMORY false
#define EEPROM_BEGIN_ADDRESS 0  //bytes
// reserve for storing settings
// not used if USE_EEPROM_MEMORY false
// =========================================
// ========== END OF SETTINGS ==============
// =========================================



