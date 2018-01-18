#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

/*
 * Будем работать с сектором 2
 * Сектор 0 хранит системную информацию, его лучше не трогать.
 * В теории он защищен от записи, хотя я видел печальные истории 
 * о запоротых метках при попытке записать информацию в 
 * нулевой сектор.
 * Сектор 1 может быть зарезервирован для CIS индексов, не знаю
 * что это, думаю лучше не трогать. 
 * 
 * Память метки разделена на сектора по 64 байта
 * Каждый сектор разделен на 4 блока
 * Первые три блока каждого сектора используются для хранения данных
 * Четвертый блок хранит информацию о ключах и права доступа 
 * Не зная ключа A или B сектора нельзя получить доступ к данным
 * По умолчанию ключ 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
 */

#define SECTOR 2
#define BLOCK SECTOR * 4
#define TRAILER_BLOCK SECTOR * 4 + 3

MFRC522::MIFARE_Key defaultKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
MFRC522::MIFARE_Key key = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::StatusCode status;
MFRC522::PICC_Type piccType;

/** 
 * Печатает массив байт в виде HEX значений
 * @param buffer - указатель на буффер
 * @param bufferSize - размер буфера
 */

void dumpByteArray(byte *buffer, byte bufferSize, int ascii) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  if (ascii) {
    Serial.print("  |  ");
    for (char i = 0; i < bufferSize; i++) {
      Serial.write((buffer[i] > 32 && buffer[i] < 127) ? buffer[i] : ' ');
    }
  }
}

/**
 * Инициализация
 */

void setup() {
  Serial.begin(9600);
  while (!Serial);

  SPI.begin(); // Инициализируем интерфейс обмена данными SPI
  mfrc522.PCD_Init(); // Инициализируем карту mfrc522

  Serial.print("sector: "); Serial.println(SECTOR);
  Serial.print("block: "); Serial.println(BLOCK);
  Serial.print("trailer block: "); Serial.println(TRAILER_BLOCK);
}

/**
 * Системный цикл
 */

void loop() {
  
  /*
   * Проверяем, поднесена ли карта
   */
   
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  /*
   * Пробуем прочесть серийную информацию
   */
   
  if (!mfrc522.PICC_ReadCardSerial()) {
    goto finalize;
  }

  /* 
   * Выводим серийную информацию карты 
   */
   
  Serial.print("Card UID:");
  dumpByteArray(mfrc522.uid.uidByte, mfrc522.uid.size, 0);
  Serial.println();
  Serial.print(F("PICC type: "));
  piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  /*
   * Аутентификация
   */
   
  Serial.println("Authenticate key A (0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)");
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_BLOCK, &defaultKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
	  Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    //goto finalize;
  } else {
	  
    /*
     * Записываем данные
     */

    const char data[16] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', 0};
    Serial.println("write data");
    dumpByteArray(data, sizeof(data), 1);
    Serial.println("");
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(BLOCK, data, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      goto finalize;
    }

    /*
     * Читаем записанное
     * 
     * Размер буфера должен быть на 2 байта больше требуемого так как в конец добавляется 2 байта контрольной суммы
     * Размер задается отдельной переменной, так как в нее будет занесено кол-во прочитанных байт
     */

    char readBuffer[18]; 
    char size = 18;
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(BLOCK, readBuffer, &size);
    Serial.println("read data");
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));    
      goto finalize;
    }
    dumpByteArray(readBuffer, sizeof(readBuffer), 1);  
    Serial.println();

    /*
     * Перезаписываем trailer
     */

    const char trailerData[16] = {
     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // key A
     0x00, 0x00, 0x00, // access bits
     0x00, // user data
     0x01, 0x02, 0x03, 0x04, 0x05, 0x06 // key B      
    };

    Serial.println("write trailer data");
  	status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(TRAILER_BLOCK, data, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      goto finalize;
    }

    goto finalize;
  }

   /*
    * Пробуем прочитать сектор с новым ключем
    */

  Serial.println("Authenticate key B (0x01, 0x02, 0x03, 0x04, 0x05, 0x06)");
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, TRAILER_BLOCK, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    goto finalize;
  } else {
    Serial.println("success");

    /*
     * Читаем записанное
     * 
     * Размер буфера должен быть на 2 байта больше требуемого так как в конец добавляется 2 байта контрольной суммы
     * Размер задается отдельной переменной, так как в нее будет занесено кол-во прочитанных байт
     */

    char readBuffer[18]; 
    char size = 18;
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(BLOCK, readBuffer, &size);
    Serial.println("read data");
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      goto finalize;
    }
    dumpByteArray(readBuffer, sizeof(readBuffer), 1);  
    Serial.println();
    
    goto finalize;
  }  

  finalize: {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

}

