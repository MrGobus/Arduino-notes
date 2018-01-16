#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::StatusCode status;

/*
 * Заводской ключ доступа к блокам A = 0xFFFFFF, B = 0xFFFFFF
 * Ключи A и B нужны для получения досутпа чтения записи данных
 */
 
MFRC522::MIFARE_Key key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

/** 
 * Печатает массив байт в виде HEX значений
 * @param buffer - указатель на буффер
 * @param bufferSize - размер буфера
 */

void dumpByteArray(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
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
}

/**
 * Системный цикл
 */

void loop() {
  
  /**
   * Проверяем, поднесена ли карта
   */
   
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  /**
   * Пробуем прочесть серийную информацию
   */
   
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;  
  }

  /** 
   * Выводим серийную информацию карты 
   */
   
  Serial.print("Card UID:");
  dumpByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  /**
   * Будем работать с сектором 2
   * Сектор 0 хранит системную информацию, его лучше не трогать.
   * В теории он защищен от записи, хотя я видел печальные истории 
   * о запоротых метках при попытке записать информацию в 
   * нулевой сектор.
   * Сектор 1 может быть зарезервирован для CIS индексов, незнаю
   * что это, думаю лучше не трогать. 
   * 
   * Памать метки разделена на сектора по 64 байта
   * Каждый сектор разделен на 4 блока
   * Первые три блока каждого сектора используются для хранения данных
   * Четвертый блок хранит информацию о ключах и права доступа 
   * Не зная ключа A или B сектора нельзя получить доступ к данным
   * По умолчанию ключ 0xFFFFFF
   */
  
  #define SECTOR 2
  #define BLOCK 8
  #define TRAILER_BLOCK 11

  /**
   * Аунтификация по коду B
   */

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, TRAILER_BLOCK, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;     
  }

  /**
   * Данные для записи
   */

  const char data[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', 0};

  Serial.print("access granted\n");

  // Записываем данные
  // <<--------------- КОДИТЬ ТУТ!!!

  /**
   * Сбрасываем адаптер для следующего чтения
   */
   
  mfrc522.PICC_HaltA();
 
  /** 
   *  Stop encryption on PCD
   */
   
  mfrc522.PCD_StopCrypto1();
}

