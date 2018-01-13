/**
 * Поочередно маргаем встроенными светодиодами и внешним
 * с задержкой в 1000 миллисекунд (1 секунда)
 *
 * @platform Arduino Pro Micro (Leonardo)
 * @author: MrGobus <mrgobus@gmail.com>
 */

/*
 * Пины встроенных светодиодов
 * Значения отличаются от стандартной arduino
 */

#define RX_LED_PIN 17 // Пин встроенного светодиода RX
#define TX_LED_PIN 30 // Пин встроенного светодиода TX
#define EXT_LED_PIN 3 // Пин внешнего светодиода

/*
 * По какой то, видимо технологической, причине включение
 * и выключение встроенных светодиодов инвертировано, по этому определяем
 * константы для крастоы кода.
 */

#define INNER_LED_ON 0
#define INNER_LED_OFF 1

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
  // Последовательно включаем встроенные светодиоды
  // и моргаем внешним светодиодом
  // с задержкой в 1000 миллисекунд (1 секунда)

  digitalWrite(EXT_LED_PIN, HIGH); // Включает внешний светодиод
  digitalWrite(RX_LED_PIN, INNER_LED_ON); // Включает внутренний светодиод RX
  digitalWrite(TX_LED_PIN, INNER_LED_OFF); // Выключает внутренний светодиод TX
  delay(1000); // Пауза

  digitalWrite(EXT_LED_PIN, LOW); // Выключает внешний светодиод
  digitalWrite(RX_LED_PIN, INNER_LED_OFF); // Выключает внутренний светодиод RX
  digitalWrite(TX_LED_PIN, INNER_LED_ON); // Включает внутренний светодиод TX
  delay(1000); // Пауза
}
