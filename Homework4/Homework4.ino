#define NUMBER_OF_DISPLAYS 4
#define NUMBER_OF_POSSIBLE_DIGITS 16

#define FIRST_BLINKING_INTERVAL 400
#define SECOND_BLINKING_INTERVAL 800

#define MIN_THRESHOLD 400
#define MAX_THRESHOLD 600

#define DEBOUNCE_DELAY 80
#define LONG_PRESS_TIME 1000
#define NORMAL_AKA_SHORT_PRESS_TIME 40

const int dataPin = 12,   //DS
          latchPin = 11,  // STCP
          clockPin = 10;  //SHCP

const int segD1 = 7,
          segD2 = 6,
          segD3 = 5,
          segD4 = 4;
          
const int pinSw = 2,
          pinX = A0,
          pinY = A1;
          
int displayDigits[NUMBER_OF_DISPLAYS] = {
  segD1, segD2, segD3, segD4
};

int digits[NUMBER_OF_DISPLAYS];

int digitArray[NUMBER_OF_POSSIBLE_DIGITS] = {
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

byte swState = LOW,
     lastSwState = LOW,
     switchState = HIGH;

int xValue, yValue;

bool joyMovedX = false,
     joyMovedY = false;

bool dpState = false;

bool selected = false;

int currentDisplayNumber = 0;

unsigned long dpBlink,
              lastDebounceTime,
              pressedTime,
              releasedTime;
              
void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < NUMBER_OF_DISPLAYS; ++i) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  pinMode(pinSw, INPUT_PULLUP);
  Serial.begin(9600);
}
void readXY() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
}

void loop() {
  readXY();
  incrementOrDecrementByMovingY();
  changeStateByPressDuration();
  writeNumber(currentDisplayNumber, selected);
}

void incrementOrDecrementByMovingY() {
  if (yValue < MIN_THRESHOLD && !joyMovedY && !selected) {
    if (currentDisplayNumber < 3)
      currentDisplayNumber++;
    joyMovedY = true;
  }
  if (yValue > MAX_THRESHOLD && !joyMovedY && !selected) {
    if (currentDisplayNumber > 0)
      currentDisplayNumber--;
    joyMovedY = true;
  }
  if (yValue >= MIN_THRESHOLD && yValue <= MAX_THRESHOLD && !selected)
    joyMovedY = false;
}

void resetLeds() {
  digits[0] = 0;
  digits[1] = 0;
  digits[2] = 0;
  digits[3] = 0;
  currentDisplayNumber = 0;
}

void changeStateByPressDuration() {
  swState = digitalRead(pinSw);
  if (swState != lastSwState)
    lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (swState != switchState) {
      switchState = swState;
      if (switchState == LOW)
        pressedTime = millis();
      else
        releasedTime = millis();
      long pressDuration = releasedTime - pressedTime;
      if (pressDuration > LONG_PRESS_TIME && selected == 0)
        resetLeds();
      else if (pressDuration > NORMAL_AKA_SHORT_PRESS_TIME)
        selected = !selected;
    }
  }
  if (selected) {
    if (xValue > MAX_THRESHOLD && !joyMovedX) {
      if (digits[currentDisplayNumber] < 15)
        digits[currentDisplayNumber]++;
      joyMovedX = true;
    }
    if (xValue < MIN_THRESHOLD && !joyMovedX) {
      if (digits[currentDisplayNumber] > 0)
        digits[currentDisplayNumber]--;
      joyMovedX = true;
    }
    if (xValue >= MIN_THRESHOLD && xValue <= MAX_THRESHOLD)
      joyMovedX = false;
  }
  lastSwState = swState;
}

// function for writing the digits on the display
void writeNumber(int currentDisplayNumber, int selected) {
  for (int i = 0; i < NUMBER_OF_DISPLAYS; ++i) {
    int digitToWrite = 0;
    if (i == currentDisplayNumber) { 
      if (selected) // if the digit display is selected, the dp must be on
        digitToWrite = digitArray[digits[i]] + 1; // incrementing the value with 1, corresponding with the dp led
      else { // if the digit display is not selected, the dp must blink
        digitToWrite = digitArray[digits[i]];
        if (millis() - dpBlink > FIRST_BLINKING_INTERVAL) {
          digitToWrite = digitArray[digits[i]] + 1;
        }
        if (millis() - dpBlink > SECOND_BLINKING_INTERVAL) {
          digitToWrite = digitArray[digits[i]];
          dpBlink = millis();
        }
      }
    }
    else
      digitToWrite = digitArray[digits[i]];
    writeReg(digitToWrite);
    showDigit(i);
    delay(5);
  }
}

void showDigit(int displayNumber) {
  for (int i = 0; i < NUMBER_OF_DISPLAYS; ++i)
    digitalWrite(displayDigits[i], HIGH);
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}
