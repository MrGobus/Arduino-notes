/**
 * Обмен даннами с arduino с использованием серийного порта 
 * 
 * Получаем строку через серийный порт и посылаем ее обратно =)
 */

#define BUFFER_SIZE 16

char buffer[BUFFER_SIZE + 1];
char position;
 
void setup() {
  Serial.begin(9600);
  Serial.println("Serial IO example for Arduino leonardo");
}

void loop() {
  if (Serial.available()) {
    position = 0;
    while (Serial.available()) {
      char x = Serial.read();
      if (position < BUFFER_SIZE) {
        buffer[position] = x;
        position++;
      }
    }
    buffer[position] = 0;
    Serial.println(buffer);
  }
}
