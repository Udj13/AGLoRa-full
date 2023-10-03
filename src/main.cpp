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
// #include <utils/memory/manager.h>
// #include <utils/memory/sram.h>
// #include <utils/memory/eeprom.h>
//#include <utils/crc.h>
//#include <hardware/gps/gps.h>
//#include <hardware/lora/ebyte-e220.h>
//#include <hardware/ble/ble.h>

//GPS gps(GPS_PIN_RX, GPS_PIN_TX, GPS_SPEED);
//LORA lora;
//BLE ble;
//MEMORY memory;

AGLORA aglora;

DATA loraDataPacket;

// ========== BEGIN ==========
void setup()
{
  Serial.begin(9600);
  // Start modules
//  ble.setup();    // Bluetooth Low Energy
//  gps.setup();    // GPS
//  lora.setup();   // LoRa
//  memory.setup(); // SRAM or EEPROM

  aglora.hello(); // Beautifully print Hello from AGloRa :-)
}

// ========== MAIN LOOP ==========
unsigned long _timeToSendMyLocation = millis() + DATA_PACKET_INTERVAL;
unsigned long _timeOfLastReceivedPacket;

void loop()
{
  if (_timeToSendMyLocation < millis())
  {
    aglora.updateSensors(&loraDataPacket);  // read additional sensors
 //   aglora.updateLocation(&gps, &loraDataPacket);
    //lora.send(&loraDataPacket); // send location to other trackers
    _timeToSendMyLocation += DATA_PACKET_INTERVAL;
  }
  
  
  /*
  if (lora.hasNewData(&loraDataPacket))
  { // listening and send to BLE
    if(memory.checkUnique(&loraDataPacket)) // Check the name and time of the point
    {
      memory.save(&loraDataPacket);
      ble.send(&loraDataPacket);         // upload data to app
      loraDataPacket.ttl --;
      if(loraDataPacket.ttl > 0) {
        lora.send(&loraDataPacket);
      }
    }
    
    _timeOfLastReceivedPacket = millis(); // if you got data, update the checker
  }

  // if the time checker is over some prescribed amount
  // let the user know there is no incoming data
  if (( _timeOfLastReceivedPacket) < millis() - BLE_UPDATE_INTERVAL)
  {
    ble.sendInfo();
    _timeOfLastReceivedPacket = millis();
  }

  ble.read(aglora.request); // check requests from app

  */
}