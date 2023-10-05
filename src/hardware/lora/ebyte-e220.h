#include <Arduino.h>

// ========== LORA section ==========
SoftwareSerial lora_ss(LORA_PIN_RX, LORA_PIN_TX);
LoRa_E220 e220ttl(&lora_ss, 6, 4, 5); // AUX M0 M1
// LoRa_E220 e220ttl(&lora_ss); // Config without connect AUX and M0 M1

bool listenToLORA();
void sendMyLocationToLoRa();
void sendLoRaData();
void setupLoRa();

// =========================================== Listen LORA =====================================
bool listenToLORA()
{
    if (e220ttl.available() > 1)
    {
        ResponseStructContainer rsc = e220ttl.receiveMessage(sizeof(DATA));
        if (rsc.status.code != 1)
        {
            Serial.println(rsc.status.getResponseDescription());
        }
        else
        {
            loraDataPacket = *(DATA *)rsc.data;
            rsc.close();
        }

        return true;
    }
    return false;
}

// ====================== READ GPS and SEND TO LORA ==============================
void sendMyLocationToLoRa()
{

}

// ================================ LORA =================================
void setupLoRa()
{
#if DEBUG_MODE
    Serial.println(F("[LoRa: Start EByte LoRa configuration.]"));
#endif

    lora_ss.begin(9600);
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
}

void sendLoRaData()
{
    lora_ss.listen();

#if DEBUG_MODE
    Serial.println();
    Serial.print(sizeof(loraDataPacket));
    Serial.println(F(" bytes ready to send"));
    Serial.print(F("* "));
    Serial.print(loraDataPacket.id);
    Serial.print(F("/"));
    Serial.print(loraDataPacket.lat, 6);
    Serial.print(F("/"));
    Serial.print(loraDataPacket.lon, 6);
    Serial.print(F(" - "));
    Serial.print(loraDataPacket.year);
    Serial.print(F("-"));
    Serial.print(loraDataPacket.month);
    Serial.print(F("-"));
    Serial.print(loraDataPacket.day);
    Serial.print(F(" "));
    Serial.print(loraDataPacket.hour);
    Serial.print(F(":"));
    Serial.print(loraDataPacket.minute);
    Serial.println(F(" (UTC) ... *"));
    Serial.println(F("[LoRa: sending coordinates.]"));
#endif

    e220ttl.sendMessage(&loraDataPacket, sizeof(loraDataPacket));
}
