#include <Keyboard.h>

#define BUTTON_PIN 2

int buttonPressState;

void setup() {
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	buttonPressState = !digitalRead(BUTTON_PIN);
	Keyboard.begin(); // Начинаем работать как клавиатура
}

void loop() {
  // Так как кнопка подключена на прямую 
  // без резистора в режиме INPUT_PINUP
  // пин возвращает LOW при нажатии и 
  // HIGH если кнопка отпущена. 
  // По этой причине инвертируем полученный результат
  // при чтении BUTTON_PIN
	int buttonPinState = !digitalRead(BUTTON_PIN);
	if (buttonPressState != buttonPinState) {
		buttonPressState = buttonPinState;
		if (buttonPressState) {
			Keyboard.press('x');
		} else {
			Keyboard.release('x');
		}
		delay(10); // Борьба с дребезгом
	}
}
