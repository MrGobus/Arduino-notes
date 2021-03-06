#include <Keyboard.h>

#include <SPI.h>
#include <MFRC522.h>

// Ключ сектора по умолчанию.
MFRC522::MIFARE_Key defaultKey = {
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  }
};

// Ключ сектора хранящего пароль.
MFRC522::MIFARE_Key key = {
  {
    0x2B, 0x03, 0xA7, 0x25, 0x8F, 0xD1
  }
};

// Сектор для хранения пароля.
#define SECTOR 1 

// Блок для хранения пароля.
#define BLOCK SECTOR * 4

// Трейлер блока хранения пароля.
#define TRAILER_BLOCK SECTOR * 4 + 3 

// Данные аутентификации для блока хранящего пароль
const char trailerData[16] = {
  0x2B, 0x03, 0xA7, 0x25, 0x8F, 0xD1, // Ключ A.
  0xFF, 0x07, 0x80, // Биты доступа.
  0x69, // Резерв. 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // Ключ B.
};

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Статус выполнения функций MFRC522
MFRC522::StatusCode status;

// Режим приложения:
// 0 - ожидание карты либо пароля из com порта для установки.
// 1 - ожидание карты для записи пароля (вывод информации о запросе).
// 2 - ожидание карты для записи пароля.

char appMode = 0; 

// Буфер для нового пароля.
char newPasswordBuffer[17];

// Длинна нового пароля.
char newPasswordLength = 0; 

// Состояние аутентификации.
char authState;

// Буфер для чтения.
// Длинна 18 байта - 16 байт данные и 2 байта CRC.
char readBuffer[18];

// Переменная для задания и хранения размера 
// прочитанной информации.
char rsize;

// Режим первой записи на метку.
char firstTimeWriteState;

/**
 * Инициализация.
 */
 
void setup() {
  // Инициализируем серийный порт
  Serial.begin(9600);
  
  // Инициализируем интерфейс обмена данными SPI
  SPI.begin();

  // Инициализируем mfrc522
  mfrc522.PCD_Init(); 

  // Инициализации клавиатуры
  Keyboard.begin();
}

/**
 * Программный цикл.
 */

void loop() {

  if ((appMode == 1) || (appMode == 2)) { // ожидание карты для записи пароля.

    // Выход из режима записи пароля.
    // Любой символ введенный в консоль.
    if (Serial.available()) {
      while(Serial.available()) {
        Serial.read();
      }
      Serial.println("password write aborted");
      appMode = 0;
      return;
    }

    // Вывод информации о режиме и ожидании карты.
    if (appMode == 1) {
      Serial.println("change password mode");
      Serial.print("password:");
      Serial.println(newPasswordBuffer);
      Serial.println("place the card");
      appMode = 2;
    }

    // Проверяем наличие карты.
    if (mfrc522.PICC_IsNewCardPresent()) {

      // Читаем серийную информацию о карте.
      if (!mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      Serial.println("card detected");

      // Аутентификация.
      // Возможные варианты, по заводскому ключу для первой записи 
      // и по программному ключу для перезаписи данных.
      authState = 0;
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_BLOCK, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {

        if (mfrc522.PICC_IsNewCardPresent()) {
          if (mfrc522.PICC_ReadCardSerial()) {
            status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_BLOCK, &defaultKey, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
              Serial.print(F("PCD_Authenticate() failed: "));
              Serial.println(mfrc522.GetStatusCodeName(status));
            } else {
              authState = 1;
            }
          }
        }
        
      } else {
        authState = 1;
      }

      // Записываем данные пароля в метку.
      if (authState) {
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(BLOCK, newPasswordBuffer, 16);
        if (status == MFRC522::STATUS_OK) {
          Serial.println("write password done");
        } else {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
        }

        // Переписываем трейлер
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(TRAILER_BLOCK, trailerData, 16);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
        }
        
        appMode = 0;
      }

      // Завершаем работу
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      
    }

  } else { // appMode == 0, ожидание карты либо пароля из com порта для установки.
    
    // Читаем пароль из серийного порта.
    if (Serial) {

      if (Serial.available()) {
        newPasswordLength = 0;
        while(Serial.available()) {
          char symbol = Serial.read();
          if ((symbol >= 0x20) && (symbol <= 0x7F)) { // Проверка на печатный символ.
            newPasswordBuffer[newPasswordLength] = symbol;
            newPasswordLength++;
            if (newPasswordLength >= 16) {
              break;
            }
          } else {
            break;
          }
        }
      
        // Очищаем порт от остатков.
        while(Serial.available()) {
          Serial.read();
        }
  
        // Ставим 0 в конец строки чтобы было ASCIIZ.
        newPasswordBuffer[newPasswordLength] = 0; 
  
        // Переходим в режим записи пароля.
        if (newPasswordLength) { 
          appMode = 1;
        }
  
        return;
      }
    } // if (Serial)

    // Проверяем наличие карты.
    if (mfrc522.PICC_IsNewCardPresent()) {

      // Читаем серийную информацию о карте.
      if (!mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      // Аутентификация.
      authState = 1;
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_BLOCK, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        authState = 0;
      }

      // Считываем пароль.
      if (authState) {
        rsize = 18;
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(BLOCK, readBuffer, &rsize);
        if (status == MFRC522::STATUS_OK) {
          readBuffer[16] = 0;
          Keyboard.print(readBuffer);
        }
      }

      // Завершаем работу
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      
    }
    
  }

}
