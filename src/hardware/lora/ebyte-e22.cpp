/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2023 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/


#include "ebyte-e22.h"

#ifdef ARDUINO_AVR_EBYTE_E22 


// ========== LORA section ==========

LORA::LORA(uint8_t pinRx, uint8_t pinTx, uint8_t aux, uint8_t m0, uint8_t m1, INDICATION *indication) : loraPort(pinRx, pinTx),
                                                                                                                    e22ttl(&loraPort, aux, m0, m1)
{
    loraPort.begin(LORA_START_SPEED);
    _indication = indication;
}

void LORA::setup()
{
#if DEBUG_MODE && DEBUG_LORA
    Serial.println(F("🛜 [LORA: Start configuration]"));
#endif

    e22ttl.begin();
    e22ttl.resetModule();

    ResponseStructContainer c;
    c = e22ttl.getConfiguration();
    Configuration configuration = *(Configuration *)c.data;
    delay(100);

    configuration.ADDL = 0x03;
    configuration.ADDH = 0x00;
    configuration.NETID = 0x00;
    configuration.CHAN = 23;
    configuration.SPED.uartBaudRate = UART_BPS_19200; // new speed
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    configuration.SPED.uartParity = MODE_00_8N1;
    
    configuration.OPTION.subPacketSetting = SPS_240_00;
    configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
    configuration.OPTION.transmissionPower = POWER_30; // 21/24/27/30  if define E22_30

    configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
    configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    configuration.TRANSMISSION_MODE.enableRepeater = REPEATER_DISABLED;
    configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // monitoring before data transmitted
    configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;

    e22ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
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
    loraPort.begin(19200); 

}

void LORA::send(LORADATA *loraDataPacket)
{
    loraPort.listen();
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

    ResponseStatus rs = e22ttl.sendMessage(loraDataPacket, LORADATASIZE);

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
    if (e22ttl.available() > 1)
    {
#if DEBUG_MODE && DEBUG_LORA
        Serial.println(F("🛜 [LORA: we have new data 🥳]"));
#endif

        rsc = e22ttl.receiveMessage(sizeof(LORADATA));
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