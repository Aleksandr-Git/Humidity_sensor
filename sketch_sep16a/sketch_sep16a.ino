#include <Bounce2.h> // библиотека для обработки дребезгов контактов

Bounce debouncer_0 = Bounce();

int dPin_0 = 0;
int LEDpin = 13;

void setup() {
pinMode(dPin_0, INPUT);
digitalWrite(dPin_0, LOW);

//debouncer_0.attach(dPin_0); // Даем бибилотеке знать, к какому пину мы подключили кнопку
//debouncer_0.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
Serial.begin(9600);
}



void loop() {
  debouncer_0.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  if (digitalRead(dPin_0) == HIGH){
//  if (debouncer_0.read() == HIGH){
    digitalWrite(LEDpin, LOW);
    Serial.println("ON");
  }

//  if (debouncer_0.read() == LOW){
  if (digitalRead(dPin_0) == LOW){
    digitalWrite(LEDpin, HIGH);
    Serial.println("OFF");
  }

}
