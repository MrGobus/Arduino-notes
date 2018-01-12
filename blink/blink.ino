/*
 * Пины встроенных светодиодов
 * Значения отличаются от стандартной arduino
 */
 
#define RX_LED_PIN 17 // Пин встроенного светодиода RX
#define TX_LED_PIN 30 // Пин встроенного светодиода TX
#define EXT_LED_PIN 3 // Пин внешнего светодиода

#define LED_ON 0
#define LED_OFF 1

/*
 * Инициализация.
 */
 
void setup() {
  // Устанавливаем режим вывода для светодиодов
  pinMode(RX_LED_PIN, OUTPUT);
  pinMode(TX_LED_PIN, OUTPUT);
  pinMode(EXT_LED_PIN, OUTPUT);
}

/*
 * Выполнение. Вызывается повторно до бесконечности.
 */

void loop() {
  // Последовательно включаем встроенные светодиды 
  // с задержкой в 1000 миллисекунд (1 секунда)
  digitalWrite(EXT_LED_PIN, HIGH);
  digitalWrite(RX_LED_PIN, LED_ON);
  digitalWrite(TX_LED_PIN, LED_OFF);
  delay(1000);
  digitalWrite(EXT_LED_PIN, LOW);
  digitalWrite(RX_LED_PIN, LED_OFF);
  digitalWrite(TX_LED_PIN, LED_ON);
  delay(1000);
}

