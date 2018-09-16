// !!! скорей всего нодо будет переименовать аналоговые контакты как A0, A1 ...
// добавлено передача данных с датчиков
// добавлен отдельный поток на включение зумера

#include <Thread.h> // потоки
#include <Bounce2.h> // библиотека для обработки дребезгов контактов

// написать отдельный поток для зумера

Thread soundThread = Thread(); // поток управления сиреной
Thread sensorThread = Thread(); // поток для датчиков

int sensPin_0 = A0; // аналоговый пин датчика №0
int sensPin_1 = A1; // аналоговый пин датчика №1
int sensPin_2 = A2; // аналоговый пин датчика №2
int sensPin_3 = A3; // аналоговый пин датчика №3

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

boolean SOUND_Alarm = false;

int NORMA = 50; // порог нормы 

static int ton = 1;  // тональность звука, Гц
static bool soundStatus = false; // переменная для контроля состояния звука

Bounce debouncer_0 = Bounce();
Bounce debouncer_1 = Bounce();
Bounce debouncer_2 = Bounce();
Bounce debouncer_3 = Bounce();

const int dPin_0 = 0; // контроль электропитания вводного щита
const int dPin_1 = 1; // защита от протечки
const int dPin_2 = 2; // датчик движения
const int dPin_3 = 3; // включение режима охраны

boolean flag_0 = true;
boolean flag_1 = true;
boolean flag_2 = false;
int flag_3 = 3;

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

  pinMode(dPin_0, INPUT);
  pinMode(dPin_1, INPUT);
  pinMode(dPin_2, INPUT);
  pinMode(dPin_3, INPUT);

  debouncer_0.attach(dPin_0); // Даем бибилотеке знать, к какому пину мы подключили кнопку
  debouncer_0.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
  debouncer_1.attach(dPin_1); // Даем бибилотеке знать, к какому пину мы подключили кнопку
  debouncer_1.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
  debouncer_2.attach(dPin_2); // Даем бибилотеке знать, к какому пину мы подключили кнопку
  debouncer_2.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
  debouncer_3.attach(dPin_3); // Даем бибилотеке знать, к какому пину мы подключили кнопку
  debouncer_3.interval(5); // Интервал, в течение которого мы не буем получать значения с пина
  
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию

  Serial.begin(9600); // открываем com порт

  soundThread.onRun(sound); // назначаем потоку задачу
  soundThread.setInterval(500);// задаём интервал срабатывания, мсек
}

void loop() {

// если тревога и прошло время интервала потока, запускается поток со звуком
    if (SOUND_Alarm == true && soundThread.shouldRun()){
    soundThread.run();
    }
  
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
    SOUND_Alarm = true;
//    soundThread.run();
    ALARM_0 = true;
    countAlarm_0 = 0;    
    Serial.println("Alarm_0");  
    }

  // если счетчик тревог датчика №1 больше
  if (countAlarm_1 > 300){ 
    digitalWrite(LEDPin_1, HIGH);
//    digitalWrite(SOUNDPin, HIGH);
    SOUND_Alarm = true;  
//    soundThread.run();
    ALARM_1 = true;
    countAlarm_1 = 0;
    Serial.println("Alarm_1");
    }

  // если счетчик тревог датчика №2 больше
  if (countAlarm_2 > 300){ 
    digitalWrite(LEDPin_2, HIGH);
    //digitalWrite(SOUNDPin, HIGH);
    SOUND_Alarm = true;    
    ALARM_2 = true;
    countAlarm_2 = 0;
    Serial.println("Alarm_2");
    }

  // если счетчик тревог датчика №3 больше
  if (countAlarm_3 > 300){ 
    digitalWrite(LEDPin_3, HIGH);
    //digitalWrite(SOUNDPin, HIGH);
    SOUND_Alarm = true;   
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
//  if (digitalRead(SOUNDPin) == HIGH && digitalRead(buttonPin) == HIGH){ 
//    digitalWrite(SOUNDPin, LOW);
//    }

// при нажатии на кнопку звук выключается  
  if (digitalRead(buttonPin) == HIGH){
    SOUND_Alarm = false;
    digitalWrite(SOUNDPin, LOW);
    }

Serial.print(analogRead(sensPin_0));
Serial.print("/");
Serial.print(analogRead(sensPin_1));
Serial.print("/");
Serial.print(analogRead(sensPin_2));
Serial.print("/");
Serial.println(analogRead(sensPin_3));

int a_0 = analogRead(sensPin_0);
int a_1 = analogRead(sensPin_1);
int a_2 = analogRead(sensPin_2);
int a_3 = analogRead(sensPin_3);

//Serial.println(a_0 + a_1 + a_2 + a_3);
//delay(500);

  debouncer_0.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_1.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_2.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_3.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
//Serial.println(analogRead(analogInPin));
//dValue = digitalRead(dPin_0);

  if (debouncer_0.read() == HIGH && flag_0 == true){ // если значение HIGH
    Serial.println("Norma_D0");
    flag_0 = false;
    }
  if (debouncer_0.read() == LOW && flag_0 == false){ // если значение LOW
    Serial.println("Alarm_D0");
    flag_0 = true;
    }

  if (debouncer_1.read() == HIGH && flag_1 == true){ // если значение HIGH
    Serial.println("Norma_D1");
    flag_1 = false;
    }
  if (debouncer_1.read() == LOW && flag_1 == false){ // если значение LOW
    Serial.println("Alarm_D1");
    flag_1 = true;
    }

// если реле датчика движение замкнуто (значение HIGH)
  if (debouncer_2.read() == HIGH && flag_2 == true && flag_3 == 0){
    Serial.println("Norma_D2");
    flag_2 = false;
    }

// если реле датчика движения разомкнуто (значение LOW) 
  if (debouncer_2.read() == LOW && flag_2 == false && flag_3 == 0){
    Serial.println("Alarm_D2");
    flag_2 = true;
    }

// включаем режим мониторинга
  if (debouncer_3.read() == HIGH && flag_3 == 0){
    Serial.println("Mode_M");
    flag_3 = 1;
//    digitalWrite(LED, LOW);
  }

  else if (debouncer_3.read() == LOW && flag_3 == 1){
    flag_3 = 2;
  }

// включаем режим тревоги  
  else if (debouncer_3.read() == HIGH && flag_3 == 2 && debouncer_2.read() == HIGH){
    delay(3000);
    Serial.println("Mode_A");
    flag_2 = false;
    flag_3 = 3;
//    digitalWrite(LED, HIGH);
  }

  else if (debouncer_3.read() == LOW && flag_3 == 3){
    flag_3 = 0;
//    digitalWrite(LED, HIGH);
  }

// если датчик движения в тревоге, контроллер не встанет на охрану
  else if (debouncer_3.read() == HIGH && flag_3 == 2 && debouncer_2.read() == LOW){
    Serial.println("ERROR_Mode_A");
    flag_3 = 1;
  }

}

// поочередно включает и выключает звук
void sound(){
  soundStatus = !soundStatus;
  digitalWrite(SOUNDPin, soundStatus);
}

  
