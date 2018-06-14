//int sensPin_2 = 2; // аналоговый пин датчика №2
//int sensPin_3 = 3; // аналоговый пин датчика №3
int sensPin_0 = 0; // аналоговый пин датчика №0
int sensPin_1 = 1; // аналоговый пин датчика №1

int SOUNDPin = 11; // пин для звукового сигнала
int LEDPin_0 = 13; // цифровой порт светодиода №0
int LEDPin_1 = 13; // цифровой порт светодиода №1

int buttonPin = 7; // пин для кнопки сброса

int countAlarm_0 = 0; // счетчик тревог по пину №0
int countAlarm_1 = 0; // счетчик тревог по пину №1
int countNorma_0 = 0; // счетчтк норма по пину №0
int countNorma_1 = 0; // счетчтк норма по пину №1

boolean ALARM_0 = false; // состояние шлейфа, тревога или норма по пину №0
boolean ALARM_1 = false; // состояние шлейфа №1
int NORMA = 50; // порог нормы 

void setup() {
  pinMode(sensPin_0, INPUT);
  pinMode(SOUNDPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(LEDPin_0, OUTPUT);
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию

//  Serial.begin(9600); // открываем com порт
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

  // если счетчик тревог больше
  if (countAlarm_0 > 30000){ 
    digitalWrite(LEDPin_0, HIGH);
    digitalWrite(SOUNDPin, HIGH);
    ALARM_0 = true;
    countAlarm_0 = 0;
    }

  // если показание датчика ниже порога и система находится в состоянии тревоги
  if (analogRead(sensPin_0) < NORMA && ALARM_0 == true){
    countNorma_0 +=1; // увеличиваем счетчик норм
    }

  // если счетчик нормы не достигает необходимого значения и датчик показывает тревогу
  else if (analogRead(sensPin_0) > NORMA && ALARM_0 == true){ 
    countNorma_0 = 0; // присваиваеи счетчику значение 0
    }

  // если счетчик нормы больше
  if (countNorma_0 > 30000){ 
    digitalWrite(LEDPin_0, LOW);
    ALARM_0 = false;
    countNorma_0 = 0;
    }

  if (digitalRead(SOUNDPin) == HIGH && digitalRead(buttonPin) == HIGH){ // если звук в тревоге, выключаем его по нажатию кнопки
    digitalWrite(SOUNDPin, LOW);
    }
//Serial.println(analogRead(sensPin));
//delay(500);
}
