#define BUTTON_PIN 2
#define TX_LED_PIN 30

int ledState;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
