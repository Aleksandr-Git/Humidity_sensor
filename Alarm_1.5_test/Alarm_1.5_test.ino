// пределано под nano. датчики с контактов 0 и 1 переподключены на контакты 5 и 6
// !! нужно устанавливать именно Arduino Thread https://soltau.ru/index.php/arduino/item/373-kak-vypolnyat-parallelnye-zadachi-threads-v-programme-dlya-arduino
// написать комментарии
// добавлена возможность ставить и снимать с охраны по команде через серийный порт
// добавлено передача данных с датчиков
// добавлен отдельный поток на включение зумера
// переименованы аналоговые входы с 1 на А1 и т.д.

#include <Thread.h> // потоки
#include <Bounce2.h> // библиотека для обработки дребезгов контактов
#include <OneWire.h> // библиотека для работы с протоколом 1-Wire
#include <DallasTemperature.h> // библиотека для работы с датчиком DS18B20

Thread soundThread = Thread(); // поток управления сиреной
Thread sensorThread = Thread(); // поток для датчиков

// название пинов
int sensPin_0 = A0; // аналоговый пин датчика влажности №0
int sensPin_1 = A1; // аналоговый пин датчика влажности №1
int sensPin_2 = A2; // аналоговый пин датчика влажности №2
int sensPin_3 = A3; // аналоговый пин датчика влажности №3

const int dPin_0 = 5; // контроль электропитания вводного щита
const int dPin_1 = 6; // защита от протечки
const int dPin_2 = 2; // датчик движения
const int dPin_3 = 3; // реле контроля ИБП
const int dPin_4 = 4; // включение режима охраны
 
int buttonPin = 7; // пин для кнопки сброса

int SOUNDPin = 8; // пин для звукового сигнала
int LEDPin_0 = 9; // цифровой пин светодиода датчика влажности №0
int LEDPin_1 = 10; // цифровой пин светодиода датчика влажности №1
int LEDPin_2 = 11; // цифровой пин светодиода датчика влажности №2
int LEDPin_3 = 12; // цифровой пин светодиода датчика влажности №3
int LED = 13; // цифровой пин светодиода режима датчика движения (мониторинг или охрана)

// счетчики тревог датчиков влажности
int countAlarm_0 = 0;
int countAlarm_1 = 0;
int countAlarm_2 = 0;
int countAlarm_3 = 0;

// счетчики норм датчиков влажности
int countNorma_0 = 0;
int countNorma_1 = 0;
int countNorma_2 = 0;
int countNorma_3 = 0;

// состояние аналоговых шлейфов, тревога или норма
boolean ALARM_0 = false;
boolean ALARM_1 = false;
boolean ALARM_2 = false;
boolean ALARM_3 = false;

// состояние цифровых шлейфов
boolean flag_0 = true; // водной щит
boolean flag_1 = true; // протечка
boolean flag_2 = false; // датчик движения
boolean flag_3 = true; // реле контроля ИБП
int flag_4 = 3; // кнопка включения режима охраны

// состояние звукового оповещателя
boolean SOUND_Alarm = false;

int NORMA = 50; // порог нормы 

static int ton = 1;  // тональность звука, Гц
static bool soundStatus = false; // переменная для контроля состояния звука

String input_Serial = ""; // строковая переменная для записи данных из серийного порта
boolean dataReady = false; // контроль готовности принятых данных

// создаем объекты библиотеки Bounce (обработка дребезга)
Bounce debouncer_0 = Bounce();
Bounce debouncer_1 = Bounce();
Bounce debouncer_2 = Bounce();
Bounce debouncer_3 = Bounce();
Bounce debouncer_4 = Bounce();

void setup() {
  pinMode(sensPin_0, INPUT); // вход датчика влажности №0
  pinMode(sensPin_1, INPUT); // вход датчика влажности №1
  pinMode(sensPin_2, INPUT); // вход датчика влажности №2
  pinMode(sensPin_3, INPUT); // вход датчика влажности №3

  pinMode(dPin_0, INPUT); // вход контроля электропитания вводного щита
  pinMode(dPin_1, INPUT); // вход защиты от протчки
  pinMode(dPin_2, INPUT); // вход датчика движения
  pinMode(dPin_3, INPUT); // вход реле контроля ИБП
  pinMode(dPin_4, INPUT); // вход кнопки постановки на охрану
  
  pinMode(SOUNDPin, OUTPUT); // выход для звукового оповещателя
  pinMode(LEDPin_0, OUTPUT); // выход светодиода №0
  pinMode(LEDPin_1, OUTPUT); // выход светолиода №1
  pinMode(LEDPin_2, OUTPUT); // выход светодиода №2
  pinMode(LEDPin_3, OUTPUT); // выход светодиода №3
  pinMode(LED, OUTPUT); // выход светодиода режима системы
  pinMode(buttonPin, INPUT); // вход для кнопки сброса

  debouncer_0.attach(dPin_0); // Даем бибилотеке знать, к какому пину мы подключили кнопку (электропитание вводного щита)
  debouncer_0.interval(5); // Интервал, в течение которого мы не буем получать значения с пина, мсек
  debouncer_1.attach(dPin_1); // Даем бибилотеке знать, к какому пину мы подключили кнопку (защита от протчки)
  debouncer_1.interval(5); // Интервал, в течение которого мы не буем получать значения с пина, мсек
  debouncer_2.attach(dPin_2); // Даем бибилотеке знать, к какому пину мы подключили кнопку (датчик движения)
  debouncer_2.interval(5); // Интервал, в течение которого мы не буем получать значения с пина, мсек
  debouncer_3.attach(dPin_3); // Даем бибилотеке знать, к какому пину мы подключили кнопку (электропитание ИБП)
  debouncer_3.interval(5); // Интервал, в течение которого мы не буем получать значения с пина, мсек
  debouncer_4.attach(dPin_4); // Даем бибилотеке знать, к какому пину мы подключили кнопку (кнопка постановки на охрану)
  debouncer_4.interval(5); // Интервал, в течение которого мы не буем получать значения с пина, мсек
  
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию (можно не писать, установлено по умолчанию)

  Serial.begin(9600); // открываем UART порт

  soundThread.onRun(sound); // назначаем потоку задачу
  soundThread.setInterval(500);// задаём интервал срабатывания, мсек
}

void loop() {
  while (Serial.available()){ // ждем появления данных на порту UART
    char incomingChar = (char)Serial.read(); // записываем данные в массив
    if (incomingChar == 'C'){ // если первый байт равен 'C'     
      dataReady = false; // меняем состояние переменной контроля принятых данных
      input_Serial = ""; // очищаем переменную от ранее принятых данных
     
      continue; // переходим на следующий цикл
    }
   
    if (incomingChar != '!'){ // если прочитанный бит не равен "!"
      input_Serial += incomingChar; // добавляем в переменную принятый бит
    }
    else{
      dataReady = true; // в противном случае данные приняты и готовы для обработки
    }
  }

// если на любом из датчиков влажности тревога и прошло время интервала потока, запускается поток со звуком
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
// если показания датчика выше порога и система находится в состоянии нормы  
  if (analogRead(sensPin_1) > NORMA && ALARM_1 == false){ 
    countAlarm_1 += 1; // увеличиваем счетчик тревог
    }
// если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_1) < NORMA && ALARM_1 == false){ 
    countAlarm_1 = 0; // присваиваем счетчику значение 0
    }
// если показания датчика выше порога и система находится в состоянии нормы 
  if (analogRead(sensPin_2) > NORMA && ALARM_2 == false){ 
    countAlarm_2 += 1; // увеличиваем счетчик тревог
    }
// если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_2) < NORMA && ALARM_2 == false){ 
    countAlarm_2 = 0; // присваиваем счетчику значение 0 (сбрасываем)
    }
// если показания датчика выше порога и система находится в состоянии нормы
  if (analogRead(sensPin_3) > NORMA && ALARM_3 == false){ 
    countAlarm_3 += 1; // увеличиваем счетчик тревог
    }
// если счетчик тревог не достигает необходимого значения и датчик показывает норму
   else if (analogRead(sensPin_3) < NORMA && ALARM_3 == false){ 
    countAlarm_3 = 0; // присваиваем счетчику значение 0
    }

  // если счетчик тревог датчика №0 больше
  if (countAlarm_0 > 300){ 
    digitalWrite(LEDPin_0, HIGH); // включаем светодиод
    SOUND_Alarm = true; // меняем значение переменной звукового ововещения
    ALARM_0 = true; // меняем значение переменной шлейфа
    countAlarm_0 = 0; // сбрасываем счетчик
    Serial.println("Alarm_0"); // отправляем данные по серийному порту
    }

  // если счетчик тревог датчика №1 больше
  if (countAlarm_1 > 300){ 
    digitalWrite(LEDPin_1, HIGH); // включаем светодиод
    SOUND_Alarm = true; // меняем значение переменной звукового ововещения  
    ALARM_1 = true; // меняем значение переменной шлейфа
    countAlarm_1 = 0; // сбрасываем счетчик
    Serial.println("Alarm_1"); // отправляем данные по серийному порту
    }

  // если счетчик тревог датчика №2 больше
  if (countAlarm_2 > 300){ 
    digitalWrite(LEDPin_2, HIGH); // включаем светодиод
    SOUND_Alarm = true; // меняем значение переменной звукового ововещения  
    ALARM_2 = true; // меняем значение переменной шлейфа
    countAlarm_2 = 0; // сбрасываем счетчик
    Serial.println("Alarm_2"); // отправляем данные по серийному порту
    }

  // если счетчик тревог датчика №3 больше
  if (countAlarm_3 > 300){ 
    digitalWrite(LEDPin_3, HIGH); // включаем светодиод
    SOUND_Alarm = true; // меняем значение переменной звукового ововещения  
    ALARM_3 = true; // меняем значение переменной шлейфа
    countAlarm_3 = 0; // сбрасываем счетчик
    Serial.println("Alarm_3"); // отправляем данные по серийному порту
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
    digitalWrite(LEDPin_0, LOW); // выключаем светодиод
    ALARM_0 = false; // меняем значение переменной шлейфа
    countNorma_0 = 0; // сбрасываем счетчик
    Serial.println("Norma_0"); // отправляем данные по серийному порту
    }

  // если счетчик нормы датчика №1 больше
  if (countNorma_1 > 300){ 
    digitalWrite(LEDPin_1, LOW); // выключаем светодиод
    ALARM_1 = false; // меняем значение переменной шлейфа
    countNorma_1 = 0; // сбрасываем счетчик
    Serial.println("Norma_1"); // отправляем данные по серийному порту
    }

  // если счетчик нормы датчика №2 больше
  if (countNorma_2 > 300){ 
    digitalWrite(LEDPin_2, LOW); // выключаем светодиод
    ALARM_2 = false; // меняем значение переменной шлейфа
    countNorma_2 = 0; // сбрасываем счетчик
    Serial.println("Norma_2");
    }

  // если счетчик нормы датчика №3 больше
  if (countNorma_3 > 300){ 
    digitalWrite(LEDPin_3, LOW); // выключаем светодиод
    ALARM_3 = false; // меняем значение переменной шлейфа
    countNorma_3 = 0; // сбрасываем счетчик
    Serial.println("Norma_3"); // отправляем данные по серийному порту
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
	
// постоянная отправка данный с датчиков влажности
Serial.print(analogRead(sensPin_0));
Serial.print("/");
Serial.print(analogRead(sensPin_1));
Serial.print("/");
Serial.print(analogRead(sensPin_2));
Serial.print("/");
Serial.println(analogRead(sensPin_3));

//int a_0 = analogRead(sensPin_0);
//int a_1 = analogRead(sensPin_1);
//int a_2 = analogRead(sensPin_2);
//int a_3 = analogRead(sensPin_3);

//Serial.println(a_0 + a_1 + a_2 + a_3);
//delay(500);

  debouncer_0.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_1.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_2.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_3.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop
  debouncer_4.update(); // Даем объекту бибилотеки знать, что надо обновить состояние - мы вошли в новый цкил loop  
//Serial.println(analogRead(analogInPin));
//dValue = digitalRead(dPin_0);

  if (debouncer_0.read() == HIGH && flag_0 == true){ // если значение HIGH (вводной щит)
    Serial.println("Norma_D0");
    flag_0 = false;
    }
	
  if (debouncer_0.read() == LOW && flag_0 == false){ // если значение LOW (вводной щит)
    Serial.println("Alarm_D0");
    flag_0 = true;
    }

  if (debouncer_1.read() == HIGH && flag_1 == true){ // если значение HIGH (Протечка)
    Serial.println("Norma_D1");
    flag_1 = false;
    }
	
  if (debouncer_1.read() == LOW && flag_1 == false){ // если значение LOW (Протечка)
    Serial.println("Alarm_D1");
    flag_1 = true;
    }

// если реле датчика движение замкнуто (значение HIGH)
  if (debouncer_2.read() == HIGH && flag_2 == true && flag_4 == 0){
    Serial.println("Norma_D2");
    flag_2 = false;
    }

// если реле датчика движения разомкнуто (значение LOW) 
  if (debouncer_2.read() == LOW && flag_2 == false && flag_4 == 0){
    Serial.println("Alarm_D2");
    flag_2 = true;
    }

  if (debouncer_3.read() == HIGH && flag_3 == true){ // если значение HIGH (реле контроля ИБП)
    Serial.println("Norma_D3");
    flag_3 = false;
    }
  
  if (debouncer_3.read() == LOW && flag_3 == false){ // если значение LOW (реле контроля ИБП)
    Serial.println("Alarm_D3");
    flag_3 = true;
    }

// включаем режим мониторинга
// в этом режиме не отправляются данные о состоянии датчика движения
  if (debouncer_4.read() == HIGH && flag_4 == 0){
    Serial.println("Mode_M");
    flag_4 = 1;
    digitalWrite(LED, LOW);
  }

  else if (debouncer_4.read() == LOW && flag_4 == 1){
    flag_4 = 2;
  }

// включаем режим тревоги
// в этом режимет отправляются данные от датчика движения
  else if (debouncer_4.read() == HIGH && flag_4 == 2 && debouncer_2.read() == HIGH){
    delay(3000); // задержка, чтобы покинуть помещение
    Serial.println("Mode_A");
    flag_2 = false;
    flag_4 = 3;
    digitalWrite(LED, HIGH);
  }

  else if (debouncer_4.read() == LOW && flag_4 == 3){
    flag_4 = 0;
    digitalWrite(LED, HIGH);
  }

// если датчик движения в тревоге, контроллер не встанет на охрану
  else if (debouncer_4.read() == HIGH && flag_4 == 2 && debouncer_2.read() == LOW){
    Serial.println("ERROR_Mode_A");
    flag_4 = 1;
  }

// включение режима мониторинга дистанционно
  else if (dataReady && input_Serial == "Mode_M" && flag_4 == 0){
    Serial.println("Mode_M");
    dataReady = false;
    flag_4 = 2;
    digitalWrite(LED, LOW);
    }

// включаем режим тревоги дистанционно
  else if (dataReady && input_Serial == "Mode_A" && flag_4 == 2 && debouncer_2.read() == HIGH){
    delay(3000); // задержка, чтобы покинуть помещение
    Serial.println("Mode_A");
    dataReady = false;
    flag_2 = false;
    flag_4 = 0;
    digitalWrite(LED, HIGH);
    }
    
// если датчик движения в тревоге, контроллер дистанционно не встанет на охрану
  else if (dataReady && input_Serial == "Mode_A" && flag_4 == 2 && debouncer_2.read() == LOW){
    Serial.println("ERROR_Mode_A");
    dataReady = false;
    flag_4 = 1;      
    }
}

// поочередно включает и выключает звук при срабртке датчиков влажности
void sound(){
  soundStatus = !soundStatus;
  digitalWrite(SOUNDPin, soundStatus);
}
