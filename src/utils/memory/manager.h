
void storageInit();

void addNewDataToStorage(DATA *newData);
void clearAllPositions();

void writeTrackerInfoToBLE();
void writePositionToBLE(String *command);
void writeNewPositionToBLE();
void writeAllPositionsToBLE();


// =============================== DATA STORAGE ========================================
void storageManager(String * command) {
  #if DEBUG_MODE
    Serial.println();
    Serial.print(F("[Bluetooth input: New command received <"));
    Serial.print(*command);
    Serial.println(F(">.]"));
  #endif

  if (command->startsWith(F("info"))) { writeTrackerInfoToBLE(); return;}
  if (command->startsWith(F("all"))) { writeAllPositionsToBLE(); return;}
  if (command->startsWith(F("id="))) { writePositionToBLE(command); return;}
  if (command->startsWith(F("clear"))) { clearAllPositions(); return;}
  if (command->startsWith(F("restart"))) { storageInit(); return;}


  #if DEBUG_MODE
    Serial.println(F("[Storage: Unknown command.]"));
  #endif  
}








#if USE_EEPROM_MEMORY
// ===================== EEPROM MEMORY ===========================
  #include <EEPROM.h>

  struct EEPROMDATA {
     unsigned char counter;
     DATA data;
     unsigned char crc;
  };

  EEPROMDATA EEPROMdata;
  
  unsigned int EEPROMStorageIndex = 0;  // index in memory (address = EEPROMStorageIndex * EEPROMDataSize)  
  unsigned int incrementCounter = 0;             // min 0, max 254 (because default EEPROM is 255)

  const unsigned char EEPROMDataSize = sizeof(EEPROMDATA);
  const unsigned int EEPROMStorageSize = ( EEPROM.length() - EEPROM_BEGIN_ADDRESS ) / EEPROMDataSize;

  void storageInit(){
    EEPROMStorageIndex = 0;
    incrementCounter = 0;

    #if DEBUG_MODE 
      Serial.print(F("[Storage: Start EEPROM initialization. Size of memory: "));
      Serial.print(EEPROM.length());
      Serial.println(F(" bytes]"));
      Serial.print(F("[Storage: "));
      Serial.print(EEPROMStorageSize);
      Serial.println(F(" track points can be saved.]"));
      Serial.print(F("Finding the index of the last record. Read memory... "));
    #endif


    unsigned int prevIncCounter = 0;

    #if DEBUG_MODE 
      for(int i = 0; i < EEPROMStorageSize; i++){
        EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

        unsigned char crc = calculateCRC((unsigned char*)&EEPROMdata.data, sizeof(EEPROMdata.data));

        Serial.print(F(", "));
        if( (i % 10) == 0 ){
          Serial.println(F("*"));
        }
        Serial.print(EEPROMdata.counter);

        if( EEPROMdata.counter != 255 ){
          if( crc == EEPROMdata.crc) {
          Serial.print(F("(CRC OK)"));
          } else {
          Serial.print(F("(CRC ERROR)"));
          }
        }

      }
      Serial.println();
    #endif

    for(int i = 0; i < EEPROMStorageSize; i++){
      EEPROM.get(i * EEPROMDataSize  + EEPROM_BEGIN_ADDRESS, EEPROMdata);

      if( EEPROMdata.counter == 255 ){      // check empty EEPROM cell
        if( i == 0 ) {break;}
        EEPROMStorageIndex = i;
        incrementCounter = prevIncCounter+1;
        if(incrementCounter >= 255) {incrementCounter = 0;}
        break;
      }

      if ( abs(EEPROMdata.counter - prevIncCounter) > 1 ) { // not in sequence
        if( i != 0 ){
          if( prevIncCounter != 254 ){    // exclude the option ...252,253,254,0,1,2...
            EEPROMStorageIndex = i;
            incrementCounter = prevIncCounter+1;
            if(incrementCounter >= 255) {incrementCounter = 0;}
            break;
          }
        }
      }
      prevIncCounter = EEPROMdata.counter;
    }

    #if DEBUG_MODE 
      Serial.println();
      Serial.print(F("Next record will be "));
      Serial.println(EEPROMStorageIndex);
    #endif

  }

  void writeTrackerInfoToBLE() {
    #if DEBUG_MODE
      Serial.println();
      Serial.println(F("[LoRa: No new LORA data]"));
      Serial.println(F("[Bluetooth output: Send AGLoRa tracker's info.]"));
    #endif
    
    Serial.print(F("AGLoRa-info"));    // signature 6 bytes
    Serial.print(F("&ver=2.0"));  // BLE protocol version
    Serial.print(F("&myId="));    // tracker's name
    Serial.print(MY_NAME);
    Serial.print(F("&memorySize="));  // storage length
    Serial.print(EEPROMStorageSize);
    Serial.print(F("&memoryIndex="));  // storage length
    Serial.print(EEPROMStorageIndex);
    Serial.println(F("|"));                // record separator

  }

  // ADD NEW DATA TO STORAGE
  void addNewDataToStorage(DATA *newData) {
    EEPROMdata.counter = incrementCounter;
    EEPROMdata.data = *newData;
    EEPROMdata.crc = calculateCRC((unsigned char*)newData, sizeof(DATA));
    EEPROM.put(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

    #if DEBUG_MODE
      Serial.print(F("[Storage: New data added. Address: "));
      Serial.print(EEPROMStorageIndex * EEPROMDataSize + EEPROM_BEGIN_ADDRESS);
      Serial.print(F(", index: "));
      Serial.print(EEPROMStorageIndex);
      Serial.print(F(", counter: "));
      Serial.print(EEPROMdata.counter);
      Serial.print(F(", CRC: "));
      Serial.print(EEPROMdata.crc);
      Serial.println(F("]"));
    #endif


    EEPROMStorageIndex++;
    if (EEPROMStorageIndex >= EEPROMStorageSize) {
      EEPROMStorageIndex = 0;
    }

    incrementCounter++;
    if ( incrementCounter >= 255 ) {
      incrementCounter = 0;
    }
  }

  void writeNewPositionToBLE() {
    #if DEBUG_MODE 
      Serial.println("[Bluetooth output: Send new data to BLE.]");
    #endif

      Serial.print(F("AGLoRa-newpoint&"));
      Serial.print(F("id="));
      Serial.print(EEPROMStorageIndex - 1);
      sendPackageToBluetooth(&EEPROMdata.data);
  }

  // Send all data from storage to BLE
  void writeAllPositionsToBLE() {
    #if DEBUG_MODE 
      Serial.println(F("[Bluetooth output: Response to <all> command.]"));
    #endif
    unsigned char crc = 0;
    for (int i = 0; i < EEPROMStorageSize; i++) {
      EEPROM.get(i * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);
      if( EEPROMdata.counter != 255 ){
        #if DEBUG_MODE 
          Serial.print(EEPROMdata.counter);
          Serial.print(F(" - "));
        #endif
        Serial.print(F("AGLoRa-point&"));
        Serial.print(F("id="));
        Serial.print(i);
        crc = calculateCRC((unsigned char*)&EEPROMdata.data, sizeof(EEPROMdata.data));
        if (EEPROMdata.crc == crc) {
          Serial.print(F("&CRC_OK"));
        } else {
          Serial.print(F("&CRC_ERROR"));
        }
        Serial.print(F("&"));
        sendPackageToBluetooth(&EEPROMdata.data);  //Next tracker DATA
        delay(10);
      }
    }
  }

  void writePositionToBLE(String *command) {
    int index = command->substring(3).toInt();  // length of command "id="

    #if DEBUG_MODE 
      Serial.println(F("[Bluetooth output: Response to <id=X> command.]"));
    #endif

    Serial.print(F("AGLoRa-point&"));

    if(index > EEPROMStorageSize - 1 ){Serial.println(F("ErrorIndexOutOfRange")); return;}

    EEPROM.get(index * EEPROMDataSize + EEPROM_BEGIN_ADDRESS, EEPROMdata);

    Serial.print(F("id="));
    Serial.print(index);
    const unsigned char crc = calculateCRC((unsigned char*)&EEPROMdata.data, sizeof(EEPROMdata.data));
    if (EEPROMdata.crc == crc) {
      Serial.print(F("&CRC_OK"));
    } else {
      Serial.print(F("&CRC_ERROR"));
    }
    Serial.print(F("&"));
    sendPackageToBluetooth(&EEPROMdata.data);  //Next tracker DATA
    delay(10);
  }

  void clearAllPositions(){
  #if DEBUG_MODE 
      Serial.println(F("[Storage: clearing memory.]"));
    #endif

    unsigned char memoryCell = 0;
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.get(i + EEPROM_BEGIN_ADDRESS, memoryCell);
      if( memoryCell != 255 ){  // 255 - default value
        memoryCell = 255;
        EEPROM.put(i + EEPROM_BEGIN_ADDRESS, memoryCell);
      }
    }

    EEPROMStorageIndex = 0;
    incrementCounter = 0;
    storageInit();
  }

#else
// ===================== SRAM MEMORY ===========================

#endif









