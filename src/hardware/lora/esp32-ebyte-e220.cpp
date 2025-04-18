/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#include "esp32-ebyte-e220.h"

#ifdef ESP32_C3_EBYTE_E220


// ========== LORA section ==========

const int hardware_uart = 1;

LORA::LORA(uint8_t pinRx, uint8_t pinTx, uint8_t aux, uint8_t m0, uint8_t m1, INDICATION *indication) : loraPort(hardware_uart),
                                                                                                                    e220ttl(pinRx, pinTx, &loraPort, aux, m0, m1, UART_BPS_RATE_9600)
{
    _indication = indication;
    _pinRx = pinRx;
    _pinTx = pinTx;
}

void LORA::setup()
{
#if DEBUG_MODE && DEBUG_LORA
    Serial.println(F("🛜 [LORA: Start configuration]"));
#endif

    e220ttl.begin();
    e220ttl.resetModule();

    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration *)c.data;
    delay(100);

    configuration.ADDL = 0x00;
    configuration.ADDH = 0x00;
    configuration.CHAN = 0x17;
    configuration.SPED.uartBaudRate = UART_BPS_19200;
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    configuration.SPED.uartParity = MODE_00_8N1;
    
    configuration.OPTION.subPacketSetting = SPS_200_00;
    configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
    configuration.OPTION.transmissionPower = POWER_22;

    configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // monitoring before data transmitted
    configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;

    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    delay(100);
#if DEBUG_MODE && DEBUG_LORA
    Serial.print(F("\t🛜 [LORA current config: channel = "));
    Serial.print(configuration.getChannelDescription());
    Serial.print(F(" , airDataRate = "));
    Serial.print(configuration.SPED.getAirDataRateDescription());
    Serial.print(F(" , transmissionPower = "));
    Serial.print(configuration.OPTION.getTransmissionPowerDescription());
    Serial.println(F("]"));
#endif

    loraPort.end();
    loraPort.begin(19200, SERIAL_8N1, _pinRx, _pinTx); // New UART speed
}

void LORA::send(LORADATA *loraDataPacket)
{
    const byte LORADATASIZE = sizeof(LORADATA);

#if DEBUG_MODE && DEBUG_LORA
    Serial.print(F("🛜 [LoRa: Sending 📫, "));
    Serial.print(LORADATASIZE);
    Serial.print(F(" bytes are ready to send"));
    Serial.print(F(" ➜ "));
    Serial.print(loraDataPacket->data.name);
    Serial.print(F(" / "));
    Serial.print(loraDataPacket->data.lat, 6);
    Serial.print(F(" "));
    Serial.print(loraDataPacket->data.lon, 6);
    Serial.print(F(" / "));
    Serial.print(loraDataPacket->data.year);
    Serial.print(F("-"));
    if (loraDataPacket->data.month < 10)
        Serial.print(F("0"));
    Serial.print(loraDataPacket->data.month);
    Serial.print(F("-"));
    if (loraDataPacket->data.day < 10)
        Serial.print(F("0"));
    Serial.print(loraDataPacket->data.day);
    Serial.print(F(" "));
    Serial.print(loraDataPacket->data.hour);
    Serial.print(F(":"));
    if (loraDataPacket->data.minute < 10)
        Serial.print(F("0"));
    Serial.print(loraDataPacket->data.minute);
    Serial.print(F(" (UTC)"));
    Serial.print(F(" TTL="));
    Serial.print(loraDataPacket->ttl);
    Serial.print(F("] ➜ "));

#endif

    ResponseStatus rs = e220ttl.sendMessage(loraDataPacket, LORADATASIZE);

#if DEBUG_MODE && DEBUG_LORA
    Serial.print(F("[Status: "));
    Serial.print(rs.getResponseDescription());
#endif

    if (rs.code == 1)
    {
#if DEBUG_MODE && DEBUG_LORA
        Serial.print(F(" 🆗"));
#endif
        _indication->lora(LoRaStatuses::dataTransmitted);
    }
    else
    {
#if DEBUG_MODE && DEBUG_LORA
        Serial.print(F(" 🚨"));
#endif
        _indication->lora(LoRaStatuses::error);
    }

#if DEBUG_MODE && DEBUG_LORA
    Serial.println(F("]"));
    Serial.println();
#endif
}

bool LORA::hasNewData(LORADATA *loraDataPacket)
{
    if (e220ttl.available() > 1)
    {
        int sizeOfLoraData = sizeof(LORADATA);
        rsc = e220ttl.receiveMessage(sizeOfLoraData);

#if DEBUG_MODE && DEBUG_LORA
        Serial.println(F("🛜 [LORA: we have new data 🥳]"));

        // Serial.print(F("Buffer size: "));
        // Serial.print(sizeOfLoraData);

        // Serial.print(" | Data (raw bytes):");
        // uint8_t* dataBytes = (uint8_t*)rsc.data;
        // for (size_t i = 0; i < sizeOfLoraData; i++) {
        //     Serial.print(dataBytes[i], HEX);
        //     Serial.print(" ");
        // }
        // Serial.println();

        // memcpy(loraDataPacket, (LORADATA *)rsc.data, sizeOfLoraData);
        // Serial.print(F(" Name: "));
        // Serial.println(loraDataPacket->data.name);
        // Serial.print(F(" Time: "));
        // Serial.print(loraDataPacket->data.hour);Serial.print(F(":"));
        // Serial.print(loraDataPacket->data.minute);Serial.print(F(":"));
        // Serial.println(loraDataPacket->data.second);
        // Serial.print(F(" Date: "));
        // Serial.print(loraDataPacket->data.day);Serial.print(F("-"));
        // Serial.print(loraDataPacket->data.month);Serial.print(F("-"));
        // Serial.println(loraDataPacket->data.year);
        // Serial.print(F(" Coordinames: "));
        // Serial.print(F("   Lat "));Serial.print(loraDataPacket->data.lat);
        // Serial.print(F(", Lon "));Serial.print(loraDataPacket->data.lon);
        // Serial.print(F(", valid:"));Serial.println(loraDataPacket->data.gpsValid);

        // Serial.print(F(" Battery: "));
        // Serial.println(loraDataPacket->data.battery);

#endif

        if (rsc.status.code != 1)
        {
#if DEBUG_MODE && DEBUG_LORA
            Serial.println(F("🛜 [LORA error: ❌ status - "));
            Serial.println(rsc.status.getResponseDescription());
            Serial.println(F("]"));
#endif
            _indication->lora(LoRaStatuses::error);
            return false;
        }
        else
        {
            memcpy(loraDataPacket, (LORADATA *)rsc.data, sizeof(LORADATA));
            rsc.close();
        }
        _indication->lora(LoRaStatuses::dataReceived);
        return true;
    }
    return false;
}

#endif