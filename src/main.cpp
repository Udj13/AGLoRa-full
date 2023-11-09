// First, open "settings.h" and setup

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Everything below this line is a sketch
  that will not need to be changed in most cases
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#include "settings/settings.h"
#include "aglora/aglora.h"
#include <hardware/gps/gps.h>
#include <hardware/lora/ebyte-e220.h>
#include <hardware/lora/loraData.h>
#include <hardware/ble/hm-10.h>
#include "tests/tests.h"
#include "utils/memory/sram/sram.h"
// #include "utils/memory/eeprom/eepromaglora.h"
//  #include <utils/crc.h>

TESTS tests;
GPS gps(GPS_PIN_RX, GPS_PIN_TX, GPS_SPEED, GPS_LED);
LORA lora(LORA_PIN_RX, LORA_PIN_TX, LORA_SPEED, LORA_PIN_AX, LORA_PIN_M0, LORA_PIN_M1, LORA_LED);
BLE_HM10 ble;

#if USE_EEPROM_MEMORY
EEPROMAglora memory;
#else
SRAM memory;
#endif

DATA trackerData;
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

  loraDataPackage.data = &trackerData;
}

// ========== MAIN LOOP ==========
unsigned long _timeToSendMyLocation = millis() + DATA_SENDING_INTERVAL;
unsigned long _timeOfLastReceivedPacket;
unsigned int addedMemoryIndex;
byte ttl = 0;

void processNewData(LORADATA * loraDataPackage);

void loop()
{
  if (_timeToSendMyLocation < millis())
  {
    aglora.clearDataPacket(loraDataPackage.data); // clear structure before reading new data
    aglora.updateSensors(loraDataPackage.data);   // add sensors
    gps.updateLocation(loraDataPackage.data);     // add locations
    loraDataPackage.ttl = TTL; // time to live (for mesh network)

    aglora.printPackage(&loraDataPackage);

    lora.send(&loraDataPackage); // send location to other trackers
    _timeToSendMyLocation += DATA_SENDING_INTERVAL;
  }

  // waiting for new data
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
  if ((_timeOfLastReceivedPacket) < millis() - BLE_UPDATE_INTERVAL)
  {
    // ble.sendInfo();
    _timeOfLastReceivedPacket = millis();
  }

  aglora.getRequest(ble.read()); // check requests from app
}

void processNewData(LORADATA * loraDataPackage)
{
  if (memory.checkUnique(loraDataPackage->data)) // Check the name and time of the point
  {

    ttl = loraDataPackage->ttl;

    addedMemoryIndex = memory.save(loraDataPackage->data);
    memory.checkCRC();
    aglora.sendPackageToBLE(loraDataPackage->data, addedMemoryIndex); // upload data to app

    // resend data to other trackers
    if (--ttl > 0)
    {
      loraDataPackage->ttl = ttl;
      lora.send(loraDataPackage);
    }
  }

  _timeOfLastReceivedPacket = millis(); // if you got data, update the checker
}