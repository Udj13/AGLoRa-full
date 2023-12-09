/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


// First, open "settings.h" and setup

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Everything below this line is a sketch
  that will not need to be changed in most cases
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "settings/settings.h"
#include "aglora/aglora.h"
#include "hardware/gps/gps.h"
#include "hardware/lora/ebyte-e220.h"
#include "hardware/lora/loraData.h"
#include "hardware/ble/hm-10.h"
#include "tests/tests.h"
#include "utils/memory/sram/sram.h"
#include "utils/memory/eeprom/eepromaglora.h"
#include "hardware/indication/indication.h"

/*
 _ __ ___     __ _  (_)  _ __         ___   _ __    _ __
 | '_ ` _ \   / _` | | | | '_ \       / __| | '_ \  | '_ \
 | | | | | | | (_| | | | | | | |  _  | (__  | |_) | | |_) |
 |_| |_| |_|  \__,_| |_| |_| |_| (_)  \___| | .__/  | .__/
                                            |_|     |_|
*/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

TESTS tests;
INDICATION indication(GPS_LED, LORA_LED, BLE_LED, MEMORY_LED);
GPS gps(GPS_PIN_RX, GPS_PIN_TX, GPS_SPEED, &indication);
LORA lora(LORA_PIN_RX, LORA_PIN_TX, LORA_SPEED, LORA_PIN_AX, LORA_PIN_M0, LORA_PIN_M1, &indication);
BLE_HM10 ble;

#if USE_EEPROM_MEMORY
EEPROMAglora memory;
#else
SRAM memory;
#endif

LORADATA loraDataPackage;
AGLORA aglora(&memory, &ble);

// ========== BEGIN ==========
void setup()
{
  Serial.begin(9600);
  aglora.hello(); // Beautifully print Hello from AGloRa :-)
  // Start modules
  gps.setup();    // GPS
  lora.setup();   // LoRa
  memory.setup(); // SRAM or EEPROM
  ble.setup();    // Bluetooth Low Energy
}

// ========== MAIN LOOP ==========
unsigned long _timeToSendMyLocation = millis() + DATA_SENDING_INTERVAL;
unsigned long _timeOfLastReceivedPacket;
unsigned int addedMemoryIndex;
byte ttl = 0;

void processNewData(LORADATA *loraDataPackage);

void loop()
{
  if (_timeToSendMyLocation < millis())
  {
#if I_WANT_TO_SEND_MY_LOCATION
    aglora.clearDataPacket(&loraDataPackage.data); // clear structure before reading new data
    aglora.updateSensors(&loraDataPackage.data);   // add sensors
    gps.updateLocation(&loraDataPackage.data);     // add locations
    loraDataPackage.ttl = TTL;                     // time to live (for mesh network)

    aglora.printPackage(&loraDataPackage);

    lora.send(&loraDataPackage); // send location to other trackers
#endif

    _timeToSendMyLocation += DATA_SENDING_INTERVAL;
  }

  // checking for new data
  if (lora.hasNewData(&loraDataPackage))
  {
    processNewData(&loraDataPackage);
  }

#if TEST_LORA_DATA
  if (tests.hasNewDataEveryXSec(&loraDataPackage, &gps, 10))
  {
    processNewData(&loraDataPackage);
  }
#endif

  // if the time checker is over some prescribed amount
  // let the user know there is no incoming data
  if ((_timeOfLastReceivedPacket) + BLE_UPDATE_INTERVAL < millis() )
  {
    aglora.checkMemoryToBLE();
    _timeOfLastReceivedPacket = millis();
  }

  aglora.getRequest(ble.read()); // check requests from app

  indication.loop(); // make an indication
}

/**
 * Processes new data from a LORADATA package.
 *
 * @param loraDataPackage The LORADATA package containing the new data to be processed.
 *
 * @return void
 *
 * @throws None
 */
void processNewData(LORADATA *loraDataPackage)
{
  if (memory.checkUnique(&loraDataPackage->data)) // Check the name and time of the point
  {

    ttl = loraDataPackage->ttl;

    addedMemoryIndex = memory.save(&loraDataPackage->data);
    memory.checkCRC();
    aglora.sendPackageToBLE(&loraDataPackage->data, addedMemoryIndex); // upload data to app

// resend data to other trackers
#if MESH_MODE
    if (--ttl > 0)
    {
      loraDataPackage->ttl = ttl;
      lora.send(loraDataPackage);
    }
#endif
  }

  _timeOfLastReceivedPacket = millis(); // if you got data, update the checker
}