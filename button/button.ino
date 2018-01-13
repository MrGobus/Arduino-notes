/**
 * Включаем встроенный светодиод при нажатии на кнопку
 *
 * @platform Arduino Pro Micro (Leonardo)
 * @author: MrGobus <mrgobus@gmail.com>
 */

#define BUTTON_PIN 2 // Пин подключенной кнопки
#define TX_LED_PIN 30 // Пин встроенного светодиода

int ledState; // Состояние светодиода

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); //чтение с использованием внутреннего резистора
  pinMode(TX_LED_PIN, OUTPUT);
  ledState = digitalRead(BUTTON_PIN);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (ledState != buttonState) {
    ledState = buttonState;
    digitalWrite(TX_LED_PIN, ledState);
  }
}
