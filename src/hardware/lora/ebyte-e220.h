#include <Arduino.h>

// ========== LORA section ==========
DATA loraDataPacket;
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

// DEBUG_MODE
#if DEBUG_MODE // dump out what was just received
        Serial.println();
        Serial.println(F("[LoRa: New data received.]"));
        Serial.print(F("ID: "));
        Serial.print(loraDataPacket.id);
        Serial.print(F(", lat: "));
        Serial.print(loraDataPacket.lat, 6);
        Serial.print(F(", lon: "));
        Serial.print(loraDataPacket.lon, 6);
        Serial.print(F(", satellites: "));
        Serial.print(loraDataPacket.sat);

        Serial.print(F(", date: "));
        Serial.print(loraDataPacket.year);
        Serial.print(F("/"));
        Serial.print(loraDataPacket.month);
        Serial.print(F("/"));
        Serial.print(loraDataPacket.day);

        Serial.print(F(", time: "));
        Serial.print(loraDataPacket.hour);
        Serial.print(F(":"));
        Serial.print(loraDataPacket.minute);
        Serial.print(F(":"));
        Serial.print(loraDataPacket.second);
        Serial.println(F(" (UTC)"));
#endif
        return true;
    }
    return false;
}

// ====================== READ GPS and SEND TO LORA ==============================
void sendMyLocationToLoRa(GPS * gps)
{

    gps->port.listen(); // switch to gps software serial

    bool newData = false;
    // For three seconds we parse GPS data and report some key values
    for (unsigned long start = millis(); millis() - start < 3000;)
    {
        while (gps->port.available() > 0)
            if (gps->module.encode(gps->port.read()))
            {
                newData = true;
            }
    }
    digitalWrite(LED_BUILTIN, LOW);

    lora_ss.listen(); // switch to lora software serial

    if (newData && gps->module.location.isValid() && gps->module.date.isValid() && gps->module.time.isValid())
    {
        digitalWrite(LED_BUILTIN, HIGH); // GPS is valid

        // data set
        loraDataPacket.lat = gps->module.location.lat();
        loraDataPacket.lon = gps->module.location.lng();
        loraDataPacket.sat = gps->module.satellites.value();

        loraDataPacket.year = gps->module.date.year() - 2000;
        loraDataPacket.month = gps->module.date.month();
        loraDataPacket.day = gps->module.date.day();

        loraDataPacket.hour = gps->module.time.hour();
        loraDataPacket.minute = gps->module.time.minute();
        loraDataPacket.second = gps->module.time.second();

        strcpy(loraDataPacket.id, MY_NAME);

#if DEBUG_MODE
        displayGPSInfo();
#endif

        sendLoRaData();
    }
    else
    {
#if DEBUG_MODE
        Serial.println();
        Serial.println(F("[GPS: No valid data.]"));
#endif
    }

    if (gps->module.charsProcessed() < 10)
    {
#if DEBUG_MODE
        Serial.println();
        Serial.println(F("[GPS: No characters received from GPS, check wiring!]"));
#endif
    }
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
