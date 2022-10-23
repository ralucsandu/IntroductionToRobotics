// This is the third exercise from the Lab2, which asks us to blink a LED without using the delay() function.
// In order to do this, I used the millis() function.
unsigned long startMillis = 0;
unsigned long timeToBlink = 100;
const int ledPin = 13;
int ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}
void loop() {
  unsigned long actualMillis = millis();
  if (actualMillis - startMillis >= timeToBlink) {
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
    startMillis = actualMillis;
  }
  digitalWrite(ledPin, ledState);
}