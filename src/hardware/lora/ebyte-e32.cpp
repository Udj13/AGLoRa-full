/*
Project AGLoRa (abbreviation of Arduino + GPS + LoRa)
Tiny and chip LoRa GPS tracker

https://github.com/Udj13/AGLoRa/

Copyright © 2021-2025 Eugeny Shlyagin. Contacts: <shlyagin@gmail.com>
License: http://opensource.org/licenses/MIT

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty 

*/

#include "ebyte-e32.h"

#ifdef ARDUINO_AVR_EBYTE_E32 

// ========== LORA section ==========

LORA::LORA(uint8_t pinRx, uint8_t pinTx, uint8_t aux, uint8_t m0, uint8_t m1, INDICATION *indication) : loraPort(pinRx, pinTx),
                                                                                                                    e32ttl(&loraPort, aux, m0, m1)
{
    loraPort.begin(LORA_START_SPEED);
    _indication = indication;
}

void LORA::setup()
{
#if DEBUG_MODE && DEBUG_LORA
    Serial.println(F("🛜 [LORA: Start configuration]"));
#endif

    e32ttl.begin();
    e32ttl.resetModule();

    ResponseStructContainer c;
    c = e32ttl.getConfiguration();
    Configuration configuration = *(Configuration *)c.data;
    delay(100);

    configuration.ADDL = 0x0;
    configuration.ADDH = 0x1;
    configuration.CHAN = 0x17;                                             // Channel. (410 + CHAN*1MHz) MHz. Default 17H (433MHz)
    configuration.OPTION.fec = FEC_1_ON;                                   // FEC_0_OFF / FEC_1_ON (default)  - Forward Error Correction Switch
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;  // FT_TRANSPARENT_TRANSMISSION (default) / FT_FIXED_TRANSMISSION
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;      // IO_D_MODE_OPEN_COLLECTOR / IO_D_MODE_PUSH_PULLS_PULL_UPS
    //#define E32_TTL_1W // define for 1W modules
    configuration.OPTION.transmissionPower = POWER_20;                   // 21/24/27/30 dBm if define E32_TTL_1W
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;                 // 250 (default)/500/750/1000/1250/1500/1750/2000 
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;                 // AIR_DATA_RATE_000_03 - 0.3kbps
                                                                           // AIR_DATA_RATE_001_12 - 1.2kbps
                                                                           // AIR_DATA_RATE_010_24 - 2.4kbps (default)
                                                                           // AIR_DATA_RATE_011_48 - 4.8kbps
                                                                           // AIR_DATA_RATE_100_96 - 9.6kbps
                                                                           // AIR_DATA_RATE_101_192 - 19.2kbps
                                                                           // AIR_DATA_RATE_110_192 - 19.2kbps (same 101)
                                                                           // AIR_DATA_RATE_111_192 - 19.2kbps (same 101)
    configuration.SPED.uartBaudRate = UART_BPS_57600;
    configuration.SPED.uartParity = MODE_00_8N1;


    e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    delay(100);
#if DEBUG_MODE && DEBUG_LORA
    Serial.print(F("\t🛜 [LORA current config: channel = "));
    Serial.print(configuration.getChannelDescription());

    Serial.print(F(" , airDataRate = "));
    Serial.print(configuration.SPED.getAirDataRate());


    Serial.print(F(" , transmissionPower = "));
    Serial.print(configuration.OPTION.getTransmissionPowerDescription());
    Serial.println(F("]"));
#endif

    loraPort.end();
    loraPort.begin(57600);

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

    ResponseStatus rs = e32ttl.sendMessage(loraDataPacket, LORADATASIZE);

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
    if (e32ttl.available() > 1)
    {
#if DEBUG_MODE && DEBUG_LORA
        Serial.println(F("🛜 [LORA: we have new data 🥳]"));
#endif

        rsc = e32ttl.receiveMessage(sizeof(LORADATA));
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