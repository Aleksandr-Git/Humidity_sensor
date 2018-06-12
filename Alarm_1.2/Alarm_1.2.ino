//int sensPin_0 = 0; // аналоговый порт датчика №0
//int sensPin_1 = 1; // аналоговый порт датчика №1
//int sensPin_2 = 2; // аналоговый порт датчика №2
//int sensPin_3 = 3; // аналоговый порт датчика №3
int sensPin = 0;
int LEDPin = 13; // цифровой порт светодиода
int countAlarm = 0; // счетчик тревог
int countNorma = 0; // счетчтк норма
boolean ALARM = false; // состояние системы, тревога или норма
int NORMA = 50; // порог нормы 

void setup() {
  pinMode(sensPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию

  Serial.begin(9600); // открываем com порт
}

void loop() {
  if (analogRead(sensPin) > NORMA && ALARM == false){ // если показания датчика выше порога и система находится в состоянии нормы
    countAlarm += 1; // увеличиваем счетчик тревог
    }

  if (countAlarm > 5){ // если счетчик тревог больше 5
    digitalWrite(LEDPin, HIGH);
    ALARM = true;
    countAlarm = 0;
    }

  if (analogRead(sensPin) < NORMA && ALARM == true){ // если показание датчика ниже порога и система находится в состоянии тревоги
    countNorma +=1; // увеличиваем счетчик норм
    }

  if (countNorma > 5){
    digitalWrite(LEDPin, LOW);
    ALARM = false;
    countNorma = 0;
    }
    
Serial.println(analogRead(sensPin));
delay(500);
}
