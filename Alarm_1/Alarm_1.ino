int sensPin = 0;
int LEDPin = 13;

void setup() {
  pinMode(sensPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  analogReference(DEFAULT); // отчетное напряжение 5В уже по умолчанию

  Serial.begin(9600); // открываем com порт
}

void loop() {
  if (analogRead(sensPin) > 50){
    digitalWrite(LEDPin, HIGH);
    }
  else {
    digitalWrite(LEDPin, LOW);
    }
Serial.println(analogRead(sensPin));
delay(500);
}
