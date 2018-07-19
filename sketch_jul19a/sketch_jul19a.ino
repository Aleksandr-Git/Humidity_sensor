#include <Thread.h>  // подключение библиотеки ArduinoThread
const int soundPin = 3;  // переменная с номером пина пьезоэлемента
const int ledPin = 13;  // переменная с номером пина светодиода

Thread ledThread = Thread(); // создаём поток управления светодиодом
Thread soundThread = Thread(); // создаём поток управления сиреной

void setup() {
    pinMode(soundPin, OUTPUT); // объявляем пин 3 как выход.
    pinMode(ledPin, OUTPUT);   // объявляем пин 13 как выход.

    ledThread.onRun(ledBlink);  // назначаем потоку задачу
    ledThread.setInterval(1000); // задаём интервал срабатывания, мсек
    
    soundThread.onRun(sound);     // назначаем потоку задачу
    soundThread.setInterval(20); // задаём интервал срабатывания, мсек
}

void loop() {
    // Проверим, пришло ли время переключиться светодиоду:
    if (ledThread.shouldRun())
        ledThread.run(); // запускаем поток
    
    // Проверим, пришло ли время сменить тональность сирены:
    if (soundThread.shouldRun())
        soundThread.run(); // запускаем поток
}

// Поток светодиода:
void ledBlink() { 
    static bool ledStatus = false;    // состояние светодиода Вкл/Выкл
    ledStatus = !ledStatus;           // инвертируем состояние
    digitalWrite(ledPin, ledStatus);  // включаем/выключаем светодиод
}

 // Поток сирены:
void sound() { 
    static int ton = 100;  // тональность звука, Гц
    tone(soundPin, ton);  // включаем сирену на "ton" Гц
    if (ton <= 500) {  // до частоты 500 Гц 
        ton += 100;  // увеличиваем тональность сирены
    }
    else {  // по достижении 500 Гц
        ton = 100;  // сбрасываем тональность до 100 Гц
    }
}
