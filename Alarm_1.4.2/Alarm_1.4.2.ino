// добавлен отдельный поток на включение зумера, нужно разобраться как он отклюается

#include <Thread.h>

// написать отдельный поток для зумера

Thread soundThread = Thread(); // поток управления сиреной
Thread sensorThread = Thread(); // поток для датчиков

int sensPin_0 = 0; // аналоговый пин датчика №0
int sensPin_1 = 1; // аналоговый пин датчика №1
int sensPin_2 = 2; // аналоговый пин датчика №2
int sensPin_3 = 3; // аналоговый пин датчика №3

int SOUNDPin = 9; // пин для звукового сигнала
int LEDPin_0 = 10; // цифровой порт светодиода №0
int LEDPin_1 = 11; // цифровой порт светодиода №1
int LEDPin_2 = 12; // цифровоцй порт светодиод №2
int LEDPin_3 = 13; // циыфровой пин светодиода №3

int buttonPin = 7; // пин для кнопки сброса

// счетчики тревог
int countAlarm_0 = 0;
int countAlarm_1 = 0;
int countAlarm_2 = 0;
int countAlarm_3 = 0;

// счетчики норм
int countNorma_0 = 0;
int countNorma_1 = 0;
int countNorma_2 = 0;
int countNorma_3 = 0;

// состояние шлейфов, тревога или норма
boolean ALARM_0 = false;
boolean ALARM_1 = false;
boolean ALARM_2 = false;
boolean ALARM_3 = false;

int NORMA = 50; // порог нормы 

void setup() {
  pinMode(sensPin_0, INPUT);
  pinMode(sensPin_1, INPUT);
  pinMode(sensPin_2, INPUT);
  pinMode(sensPin_3, INPUT);
  
  pinMode(SOUNDPin, OUTPUT);
  pinMode(LEDPin_0, OUTPUT);
  pinMode(LEDPin_1, OUTPUT);
  pinMode(LEDPin_2, OUTPUT);
  pinMode(LEDPin_3, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию

  Serial.begin(9600); // открываем com порт

  soundThread.onRun(sound);
//  soundThread.setInterval(20);
}

void loop() {
  // если показания датчика выше порога и система находится в состоянии нормы
  if (analogRead(sensPin_0) > NORMA && ALARM_0 == false){ 
    countAlarm_0 += 1; // увеличиваем счетчик тревог
    }
  // если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_0) < NORMA && ALARM_0 == false){ 
    countAlarm_0 = 0; // присваиваем счетчику значение 0
    } 
  
  if (analogRead(sensPin_1) > NORMA && ALARM_1 == false){ 
    countAlarm_1 += 1; // увеличиваем счетчик тревог
    }
  // если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_1) < NORMA && ALARM_1 == false){ 
    countAlarm_1 = 0; // присваиваем счетчику значение 0
    }

  if (analogRead(sensPin_2) > NORMA && ALARM_2 == false){ 
    countAlarm_2 += 1; // увеличиваем счетчик тревог
    }
  // если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_2) < NORMA && ALARM_2 == false){ 
    countAlarm_2 = 0; // присваиваем счетчику значение 0
    }

  if (analogRead(sensPin_3) > NORMA && ALARM_3 == false){ 
    countAlarm_3 += 1; // увеличиваем счетчик тревог
    }
  // если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_3) < NORMA && ALARM_3 == false){ 
    countAlarm_3 = 0; // присваиваем счетчику значение 0
    }

  // если счетчик тревог датчика №0 больше
  if (countAlarm_0 > 300){ 
    digitalWrite(LEDPin_0, HIGH);
//    digitalWrite(SOUNDPin, HIGH);
    soundThread.run();
    ALARM_0 = true;
    countAlarm_0 = 0;
    Serial.println("Alarm_0");
    
    }

  // если счетчик тревог датчика №1 больше
  if (countAlarm_1 > 300){ 
    digitalWrite(LEDPin_1, HIGH);
//    digitalWrite(SOUNDPin, HIGH);
    soundThread.run();
    ALARM_1 = true;
    countAlarm_1 = 0;
    Serial.println("Alarm_1");
    }

  // если счетчик тревог датчика №2 больше
  if (countAlarm_2 > 300){ 
    digitalWrite(LEDPin_2, HIGH);
    soundThread.run();
    //digitalWrite(SOUNDPin, HIGH);
    ALARM_2 = true;
    countAlarm_2 = 0;
    Serial.println("Alarm_2");
    }

  // если счетчик тревог датчика №3 больше
  if (countAlarm_3 > 300){ 
    digitalWrite(LEDPin_3, HIGH);
    soundThread.run();
    //digitalWrite(SOUNDPin, HIGH);
    ALARM_3 = true;
    countAlarm_3 = 0;
    Serial.println("Alarm_3");
    }

  // если показание датчика №0 ниже порога и система находится в состоянии тревоги
  if (analogRead(sensPin_0) < NORMA && ALARM_0 == true){
    countNorma_0 +=1; // увеличиваем счетчик норм
    }

  // если счетчик нормы датчика №0 не достигает необходимого значения и датчик показывает тревогу
  else if (analogRead(sensPin_0) > NORMA && ALARM_0 == true){ 
    countNorma_0 = 0; // присваиваеи счетчику значение 0
    }

  // если показание датчика №1 ниже порога и система находится в состоянии тревоги
  if (analogRead(sensPin_1) < NORMA && ALARM_1 == true){
    countNorma_1 +=1; // увеличиваем счетчик норм
    }

  // если счетчик нормы датчика №1 не достигает необходимого значения и датчик показывает тревогу
  else if (analogRead(sensPin_1) > NORMA && ALARM_1 == true){ 
    countNorma_1 = 0; // присваиваеи счетчику значение 0
    }

  // если показание датчика №2 ниже порога и система находится в состоянии тревоги
  if (analogRead(sensPin_2) < NORMA && ALARM_2 == true){
    countNorma_2 +=1; // увеличиваем счетчик норм
    }

  // если счетчик нормы датчика №2 не достигает необходимого значения и датчик показывает тревогу
  else if (analogRead(sensPin_2) > NORMA && ALARM_2 == true){ 
    countNorma_2 = 0; // присваиваеи счетчику значение 0
    }

  // если показание датчика №3 ниже порога и система находится в состоянии тревоги
  if (analogRead(sensPin_3) < NORMA && ALARM_3 == true){
    countNorma_3 +=1; // увеличиваем счетчик норм
    }

  // если счетчик нормы датчика №3 не достигает необходимого значения и датчик показывает тревогу
  else if (analogRead(sensPin_3) > NORMA && ALARM_3 == true){ 
    countNorma_3 = 0; // присваиваеи счетчику значение 0
    }

  // если счетчик нормы датчика №0 больше
  if (countNorma_0 > 300){ 
    digitalWrite(LEDPin_0, LOW);
    ALARM_0 = false;
    countNorma_0 = 0;
    Serial.println("Norma_0");
    }

  // если счетчик нормы датчика №1 больше
  if (countNorma_1 > 300){ 
    digitalWrite(LEDPin_1, LOW);
    ALARM_1 = false;
    countNorma_1 = 0;
    Serial.println("Norma_1");
    }

  // если счетчик нормы датчика №2 больше
  if (countNorma_2 > 300){ 
    digitalWrite(LEDPin_2, LOW);
    ALARM_2 = false;
    countNorma_2 = 0;
    Serial.println("Norma_2");
    }

  // если счетчик нормы датчика №3 больше
  if (countNorma_3 > 300){ 
    digitalWrite(LEDPin_3, LOW);
    ALARM_3 = false;
    countNorma_3 = 0;
    Serial.println("Norma_3");
    }

	// если звук включен, выключаем его по нажатию кнопки
  if (digitalRead(SOUNDPin) == HIGH && digitalRead(buttonPin) == HIGH){ 
    digitalWrite(SOUNDPin, LOW);
    }
//Serial.println(analogRead(sensPin_0));
//delay(500);

}

void sound() { 
  static int ton = 100;  // тональность звука, Гц
  tone(SOUNDPin, ton);  // включаем сирену на "ton" Гц
  if (ton <= 500) {  // до частоты 500 Гц 
    ton += 100;  // увеличиваем тональность сирены
    }
  else {  // по достижении 500 Гц
    ton = 100;  // сбрасываем тональность до 100 Гц
    }
  }
