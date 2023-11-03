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
#include <hardware/ble/hm-10.h>
#include "tests/tests.h"
// #include <utils/memory/manager.h>
#include <utils/memory/sram/sram.h>
// #include <utils/memory/eeprom.h>
// #include <utils/crc.h>

TESTS tests;
GPS gps(GPS_PIN_RX, GPS_PIN_TX, GPS_SPEED, GPS_LED);
LORA lora(LORA_PIN_RX, LORA_PIN_TX, LORA_SPEED, LORA_PIN_AX, LORA_PIN_M0, LORA_PIN_M1, LORA_LED);
BLE_HM10 ble;
SRAM memory;

DATA loraDataPacket;
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

void loop()
{
  if (_timeToSendMyLocation < millis())
  {
    aglora.clearDataPacket(&loraDataPacket); // clear structure before reading new data
    aglora.updateSensors(&loraDataPacket);   // add sensors
    gps.updateLocation(&loraDataPacket);     // add locations
    aglora.printPackage(&loraDataPacket);
    lora.send(&loraDataPacket); // send location to other trackers
    _timeToSendMyLocation += DATA_SENDING_INTERVAL;
  }

  // waiting for new data
  if (lora.hasNewData(&loraDataPacket))
  {
    if (memory.checkUnique(&loraDataPacket)) // Check the name and time of the point
    {
      memory.save(&loraDataPacket);
      // ble.send(&loraDataPacket);         // upload data to app

      // resend data to other trackers
      loraDataPacket.ttlOrCrc--;
      if (loraDataPacket.ttlOrCrc > 0)
      {
        lora.send(&loraDataPacket);
      }
    }

    _timeOfLastReceivedPacket = millis(); // if you got data, update the checker
  }

#if TEST_LORA_DATA
  if (tests.hasNewDataEveryXSec(&loraDataPacket, &gps, 10))
  {
    if (memory.checkUnique(&loraDataPacket)) // Check the name and time of the point
    {
      memory.save(&loraDataPacket);
      memory.checkCRC();
      aglora.sendPackageToBLE(&loraDataPacket); // upload data to app

      // resend data to other trackers
      loraDataPacket.ttlOrCrc--;
      if (loraDataPacket.ttlOrCrc > 0)
      {
        lora.send(&loraDataPacket);
      }
    }

    _timeOfLastReceivedPacket = millis(); // if you got data, update the checker
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