#include "ebyte-e220.h"

// ========== LORA section ==========

LORA::LORA(uint8_t pinRx, uint8_t pinTx, long speed, uint8_t aux, uint8_t m0, uint8_t m1, uint8_t ledPin) : loraPort(pinRx, pinTx),
                                                                                                            e220ttl(&loraPort, aux, m0, m1)
{
    loraPort.begin(speed);
    _ledPin = ledPin;
    pinMode(_ledPin, OUTPUT); // GPS valid indicator
}

void LORA::setup()
{
#if DEBUG_MODE
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
    configuration.SPED.uartBaudRate = UART_BPS_9600;
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
#if DEBUG_MODE
    Serial.print(F("🛜 [LORA current config: channel = "));
    Serial.print(configuration.getChannelDescription());
    Serial.print(F(" , airDataRate = "));
    Serial.print(configuration.SPED.getAirDataRateDescription());
    Serial.print(F(" , transmissionPower = "));
    Serial.print(configuration.OPTION.getTransmissionPowerDescription());
    Serial.println(F("]"));
#endif
}

void LORA::send(DATA *loraDataPacket)
{
    loraPort.listen();
    turnIndicatorOn();

#if DEBUG_MODE
    Serial.print(F("🛜 [LoRa: Sending 📭, "));
    Serial.print(sizeof(DATA));
    Serial.print(F(" bytes are ready to send"));
    Serial.print(F(" ➜ "));
    Serial.print(loraDataPacket->name);
    Serial.print(F("/"));
    Serial.print(loraDataPacket->lat, 6);
    Serial.print(F("/"));
    Serial.print(loraDataPacket->lon, 6);
    Serial.print(F(" - "));
    Serial.print(loraDataPacket->year);
    Serial.print(F("-"));
    Serial.print(loraDataPacket->month);
    Serial.print(F("-"));
    Serial.print(loraDataPacket->day);
    Serial.print(F(" "));
    Serial.print(loraDataPacket->hour);
    Serial.print(F(":"));
    Serial.print(loraDataPacket->minute);
    Serial.print(F(" (UTC)"));
    Serial.print(F(" TTL="));
    Serial.print(loraDataPacket->ttlOrCrc);
    Serial.println(F("]"));

#endif

    ResponseStatus rs = e220ttl.sendMessage(&loraDataPacket, sizeof(loraDataPacket));

#if DEBUG_MODE
    Serial.print(F("🛜 [LORA: Sending status ➜ "));
    Serial.print(rs.getResponseDescription());
    if (rs.code == 1)
    {
        Serial.print(F(" 🆗"));
    }
    else
    {
        Serial.print(F(" 🚨"));
    }
    Serial.println(F("]"));
    Serial.println();
#endif

    turnIndicatorOff();
}

bool LORA::hasNewData(DATA *loraDataPacket)
{
    if (e220ttl.available() > 1)
    {
#if DEBUG_MODE
        Serial.println(F("🛜 [LORA: we have new data 🥳]"));
#endif

        rsc = e220ttl.receiveMessage(sizeof(DATA));
        if (rsc.status.code != 1)
        {
#if DEBUG_MODE
            Serial.println(F("🛜 [LORA error: ❌ status - "));
            Serial.println(rsc.status.getResponseDescription());
            Serial.println(F("]"));

#endif
            return false;
        }
        else
        {
            loraDataPacket = (DATA *)rsc.data;
            rsc.close();
        }

        return true;
    }
    return false;
}

void LORA::turnIndicatorOn()
{
    digitalWrite(_ledPin, HIGH);
}

void LORA::turnIndicatorOff()
{
    digitalWrite(_ledPin, LOW);
}
