#include <Bounce2.h> // библиотека для обработки дребезгов контактов

Bounce debouncer_0 = Bounce();
Bounce debouncer_1 = Bounce();
Bounce debouncer_2 = Bounce();
Bounce debouncer_3 = Bounce();

//const int analogInPin = A0;
const int dPin_0 = 0;
const int dPin_1 = 1;
const int dPin_2 = 2;
const int dPin_3 = 3;
//int dValue;
int LED = 13;

boolean flag_0 = true;
boolean flag_1 = true;
boolean flag_2 = true;
boolean flag_3 = true;

void setup() {
pinMode(dPin_0, INPUT);
pinMode(dPin_1, INPUT);
pinMode(dPin_2, INPUT);
pinMode(dPin_3, INPUT);

pinMode(LED, OUTPUT);  // для тестов

Serial.begin(9600);


debouncer_0.attach(dPin_0); // Даем бибилотеке знать, к какому пину мы подключили кнопку
debouncer_0.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
debouncer_1.attach(dPin_1); // Даем бибилотеке знать, к какому пину мы подключили кнопку
debouncer_1.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
debouncer_2.attach(dPin_2); // Даем бибилотеке знать, к какому пину мы подключили кнопку
debouncer_2.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
debouncer_3.attach(dPin_3); // Даем бибилотеке знать, к какому пину мы подключили кнопку
debouncer_3.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
}

void loop() {
  debouncer_0.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_1.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_2.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_3.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
//Serial.println(analogRead(analogInPin));
//dValue = digitalRead(dPin_0);

  if (debouncer_0.read() == HIGH && flag_0 == true){ // если значение HIGH
    Serial.println("high_0");
    flag_0 = false;
    }
  if (debouncer_0.read() == LOW && flag_0 == false){ // если значение LOW
    Serial.println("low_0");
    flag_0 = true;
    }

  if (debouncer_1.read() == HIGH && flag_1 == true){ // если значение HIGH
    Serial.println("high_1");
    flag_1 = false;
    }
  if (debouncer_1.read() == LOW && flag_1 == false){ // если значение LOW
    Serial.println("low_1");
    flag_1 = true;
    }

  if (debouncer_2.read() == HIGH && flag_2 == true){ // если значение HIGH
    Serial.println("high_2");
    flag_2 = false;
    }
  if (debouncer_2.read() == LOW && flag_2 == false){ // если значение LOW
    Serial.println("low_2");
    flag_2 = true;
    }

  if (debouncer_3.read() == HIGH && flag_3 == true){ // если значение HIGH
    Serial.println("high_3");
    flag_3 = false;
    }
  if (debouncer_3.read() == LOW && flag_3 == false){ // если значение LOW
    Serial.println("low_3");
    flag_3 = true;
    }
//if (digitalRead(dPin_0) == HIGH){
//  digitalWrite(LED, !digitalRead(LED));
//  Serial.println("SOUND");
//  delay(200);
//  }
//else{
  //digitalWrite(LED, LOW);
  //}
}
