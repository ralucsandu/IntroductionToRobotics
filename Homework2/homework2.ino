#define STATE_1_GREEN_CARS_RED_PEOPLE_DURATION 8000
#define STATE_2_YELLOW_CARS_DURATION 3000
#define STATE_3_RED_CARS_BLINKING_GREEN_BEEP_PEOPLE_DURATION 8000
#define STATE_3_BEEPING_INTERVAL_DURATION 500
#define STATE_4_RED_CARS_BLINKING_GREEN_FASTER_BEEP_DURATION 4000
#define STATE_4_PAUSE_BETWEEN_TONES_DURATION 200
#define STATE_4_SMALLER_PAUSE_BETWEEN_TONES_DURATION 70

const int buttonPin = 2,
          buzzerPin = 5;

const int buzzerTone = 3000;

const int pedestrianGreenPin = 3,
          pedestrianRedPin = 4;

const int carGreenPin = 8,
          carYellowPin = 9,
          carRedPin = 10;

byte buttonState = LOW,
     previousButtonState = HIGH;

byte pedestrianGreenState = LOW,
     pedestrianRedState = HIGH;

byte carGreenState = HIGH,
     carYellowState = LOW,
     carRedState = LOW;

byte read = LOW,
     lastRead = LOW;

unsigned int beepCounter = 0;

unsigned int buttonPressDelay = 10;

unsigned int buttonPressBeginTime = 0,
             lastButtonPressTime = 0;

unsigned int state2BeginTime = 0,
             state3BeginTime = 0,
             state4BeginTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  pinMode(carGreenPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carRedPin, OUTPUT);

  pinMode(pedestrianGreenPin, OUTPUT);
  pinMode(pedestrianRedPin, OUTPUT);

  Serial.begin(9600);
}

void outputLeds() {
  digitalWrite(carGreenPin, carGreenState);
  digitalWrite(carYellowPin, carYellowState);
  digitalWrite(carRedPin, carRedState);

  digitalWrite(pedestrianGreenPin, pedestrianGreenState);
  digitalWrite(pedestrianRedPin, pedestrianRedState);
}


void semaphoreState1() {
  carGreenState = HIGH;
  carYellowState = LOW;
  carRedState = LOW;

  pedestrianGreenState = LOW;
  pedestrianRedState = HIGH;

  outputLeds();
}

void semaphoreState2() {

  buttonPressBeginTime = millis();

  while (millis() - buttonPressBeginTime < STATE_1_GREEN_CARS_RED_PEOPLE_DURATION) {}

  state2BeginTime = millis();

  carGreenState = LOW;
  carYellowState = HIGH;
  carRedState = LOW;

  pedestrianGreenState = LOW;
  pedestrianRedState = HIGH;

  outputLeds();

  while (millis() - state2BeginTime < STATE_2_YELLOW_CARS_DURATION) {}
}

void semaphoreState3() {
  state3BeginTime = millis();

  carGreenState = LOW;
  carYellowState = LOW;
  carRedState = HIGH;

  pedestrianGreenState = HIGH;
  pedestrianRedState = LOW;

  outputLeds();

  while (millis() - state3BeginTime < STATE_3_RED_CARS_BLINKING_GREEN_BEEP_PEOPLE_DURATION) {
    if (millis() - state3BeginTime > STATE_3_BEEPING_INTERVAL_DURATION * beepCounter)
      beepCounter++;
    if (beepCounter % 2 == 0)
      tone(buzzerPin, buzzerTone);
    else
      noTone(buzzerPin);
  }
}

void semaphoreState4() {
  state4BeginTime = millis();

  carGreenState = LOW;
  carYellowState = LOW;
  carRedState = HIGH;

  pedestrianGreenState = HIGH;
  pedestrianRedState = LOW;

  outputLeds();

  while (millis() - state4BeginTime < STATE_4_RED_CARS_BLINKING_GREEN_FASTER_BEEP_DURATION) {
    pedestrianGreenState = HIGH;
    outputLeds();
    tone(buzzerPin, buzzerTone);
    delay(STATE_4_SMALLER_PAUSE_BETWEEN_TONES_DURATION);
    noTone(buzzerPin);
    delay(STATE_4_SMALLER_PAUSE_BETWEEN_TONES_DURATION);
    pedestrianGreenState = LOW;
    outputLeds();
    tone(buzzerPin, buzzerTone);
    delay(STATE_4_SMALLER_PAUSE_BETWEEN_TONES_DURATION);
    noTone(buzzerPin);
    delay(STATE_4_SMALLER_PAUSE_BETWEEN_TONES_DURATION);
  }
}

void loop() {
  outputLeds();
  read = digitalRead(buttonPin);
  if (read != lastRead)
    lastButtonPressTime = millis();

  if (millis() - lastButtonPressTime > buttonPressDelay) {
    if (read != buttonState) {
      buttonState = read;
      if (buttonState == LOW) {
        semaphoreState2();
        semaphoreState3();
        semaphoreState4();
        semaphoreState1();
      }
    }
  }
  lastRead = read;
}