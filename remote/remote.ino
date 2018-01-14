#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

IRrecv irrecv(3); // Объект приемник

/*
 * Коды кнопок для пульта (моего)
 * Для разных пультов коды будут разные
 */

#define REMOTE_BUTTON_00 0xE318261B // CH-
#define REMOTE_BUTTON_01 0x00511DBB // CH
#define REMOTE_BUTTON_02 0xEE886D7F // CH+
#define REMOTE_BUTTON_03 0x52A3D41F // prev
#define REMOTE_BUTTON_04 0xD7E84B1B // next
#define REMOTE_BUTTON_05 0x20FE4DBB // play
#define REMOTE_BUTTON_06 0xF076C13B // vol -
#define REMOTE_BUTTON_07 0xA3C8EDDB // vol +
#define REMOTE_BUTTON_08 0xE5CFBD7F // eq
#define REMOTE_BUTTON_09 0xC101E57B // 0
#define REMOTE_BUTTON_10 0x97483BFB // 100+
#define REMOTE_BUTTON_11 0xF0C41643 // 200+
#define REMOTE_BUTTON_12 0x9716BE3F // 1
#define REMOTE_BUTTON_13 0x3D9AE3F7 // 2
#define REMOTE_BUTTON_14 0x6182021B // 3
#define REMOTE_BUTTON_15 0x8C22657B // 4
#define REMOTE_BUTTON_16 0x488F3CBB // 5
#define REMOTE_BUTTON_17 0x0449E79F // 6
#define REMOTE_BUTTON_18 0x32C6FDF7 // 7
#define REMOTE_BUTTON_19 0x1BC0157B // 8
#define REMOTE_BUTTON_20 0x3EC3FC1B // 9

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  decode_results results; // Сюда читаем код с пульта

  // Если один из кодов моего пульта, печатаем его значение
  if (irrecv.decode(&results)) {
    switch(results.value) {
      case REMOTE_BUTTON_00:
        Serial.println("CH+");
        break;
      case REMOTE_BUTTON_01:
        Serial.println("CH");
        break;
      case REMOTE_BUTTON_02:
        Serial.println("CH-");
        break;
      case REMOTE_BUTTON_03:
        Serial.println("Prev");
        break;
      case REMOTE_BUTTON_04:
        Serial.println("Next");
        break;
      case REMOTE_BUTTON_05:
        Serial.println("Play");
        break;
      case REMOTE_BUTTON_06:
        Serial.println("Vol-");
        break;
      case REMOTE_BUTTON_07:
        Serial.println("Vol+");
        break;
      case REMOTE_BUTTON_08:
        Serial.println("EQ");
        break;
      case REMOTE_BUTTON_09:
        Serial.println("0");
        break;
      case REMOTE_BUTTON_10:
        Serial.println("100+");
        break;
      case REMOTE_BUTTON_11:
        Serial.println("200+");
        break;
      case REMOTE_BUTTON_12:
        Serial.println("1");
        break;
      case REMOTE_BUTTON_13:
        Serial.println("2");
        break;
      case REMOTE_BUTTON_14:
        Serial.println("3");
        break;
      case REMOTE_BUTTON_15:
        Serial.println("4");
        break;
      case REMOTE_BUTTON_16:
        Serial.println("5");
        break;
      case REMOTE_BUTTON_17:
        Serial.println("6");
        break;
      case REMOTE_BUTTON_18:
        Serial.println("7");
        break;
      case REMOTE_BUTTON_19:
        Serial.println("8");
        break;
      case REMOTE_BUTTON_20:
        Serial.println("9");
        break;
    }

    // Печатаем код
    Serial.print("code: ");
    Serial.println( results.value, HEX );

    // Возобнавляем чтение
    irrecv.resume();
  }
}
