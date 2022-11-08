#define NUMBER_OF_SEGMENTS 8

#define MIN_THRESHOLD 400
#define MAX_THRESHOLD 600

#define LONG_PRESS_TIME 1000
#define NORMAL_AKA_SHORT_PRESS_TIME 40
#define DEBOUNCE_DELAY 80
#define BLINKING_INTERVAL 400

bool commonAnode = false;
byte commonAnodeState = HIGH;

const unsigned int pinA = 4,
                   pinB = 5,
                   pinC = 6,
                   pinD = 7,
                   pinE = 8,
                   pinF = 9,
                   pinG = 10,
                   pinDP = 11;

const int pinX = A0,
          pinY = A1;

const unsigned int pinSw = 2;

byte pinAState = LOW,
     pinBState = LOW,
     pinCState = LOW,
     pinDState = LOW,
     pinEState = LOW,
     pinFState = LOW,
     pinGState = LOW,
     pinDPState = LOW,
     currentBlinkingPinState = LOW,
     notBlinkingPinState = LOW;

unsigned int currentBlinkingPin = 11,
             stateIndex = 1;

int yValue, xValue;

byte joystickMovedXState2 = LOW,
     joystickMovedXState1 = LOW,
     joystickMovedY = LOW;

byte swState = LOW,
     switchState = HIGH,
     lastSwState = HIGH;

unsigned long numberOfPresses,
              lastDebounceTime,
              lastBlinkTime,
              pressedTime,
              releasedTime,
              timePassedSinceDebounce,
              blinkingDuration;

void setup(){
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinDP, OUTPUT);

  if (commonAnode == true)
    commonAnodeState = !commonAnodeState;

  pinMode(pinSw, INPUT_PULLUP);
  Serial.begin(9600);
}

void outputLeds(){
  digitalWrite(pinA, pinAState);
  digitalWrite(pinB, pinBState);
  digitalWrite(pinC, pinCState);
  digitalWrite(pinD, pinDState);
  digitalWrite(pinE, pinEState);
  digitalWrite(pinF, pinFState);
  digitalWrite(pinG, pinGState);
  digitalWrite(pinDP, pinDPState);
  digitalWrite(currentBlinkingPin, currentBlinkingPinState);
}

void backToInitialState(){
  currentBlinkingPin = pinDP;  
  pinAState = LOW;
  pinBState = LOW;
  pinCState = LOW;
  pinDState = LOW;
  pinEState = LOW;
  pinFState = LOW;
  pinGState = LOW;
  pinDPState = LOW;
}

void state1Blinking(){
  blinkingDuration = millis() - lastBlinkTime;
  if (blinkingDuration > BLINKING_INTERVAL && notBlinkingPinState == LOW) {
    currentBlinkingPinState = !currentBlinkingPinState;
    lastBlinkTime = millis();
  }

  bool up = (yValue > MAX_THRESHOLD && joystickMovedY == LOW);
  bool down = (yValue < MIN_THRESHOLD && joystickMovedY == LOW);
  bool left = (xValue < MIN_THRESHOLD && joystickMovedXState1 == LOW);
  bool right = (xValue > MAX_THRESHOLD && joystickMovedXState1 == LOW);
  bool xMoving = (xValue < MAX_THRESHOLD && xValue > MIN_THRESHOLD && joystickMovedXState1 == HIGH);
  bool yMoving = (yValue < MAX_THRESHOLD && yValue > MIN_THRESHOLD && joystickMovedY == HIGH);

  if (stateIndex == 1) {
    if (down) {
      if (currentBlinkingPin == pinG)
        currentBlinkingPin = pinD;
      else if (currentBlinkingPin == pinA)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinB)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinF)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinC)
        currentBlinkingPin = pinD;
      else if (currentBlinkingPin == pinE)
        currentBlinkingPin = pinD;
      joystickMovedY = !joystickMovedY;
    }
    else if (up) {
      if (currentBlinkingPin == pinG)
        currentBlinkingPin = pinA;
      else if (currentBlinkingPin == pinD)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinE)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinC)
        currentBlinkingPin = pinG;
      else if (currentBlinkingPin == pinB)
        currentBlinkingPin = pinA;
      else if (currentBlinkingPin == pinF)
        currentBlinkingPin = pinA;
      joystickMovedY = HIGH;
    } else if (yMoving)
      joystickMovedY = LOW;

    if (left) {
      if (currentBlinkingPin == pinA)
        currentBlinkingPin = pinF;
      else if (currentBlinkingPin == pinB)
        currentBlinkingPin = pinF;
      else if (currentBlinkingPin == pinC)
        currentBlinkingPin = pinE;
      else if (currentBlinkingPin == pinD)
        currentBlinkingPin = pinE;
      else if (currentBlinkingPin == pinE) {
      } 
      else if (currentBlinkingPin == pinF) {
      } 
      else if (currentBlinkingPin == pinG) {
      } 
      else if (currentBlinkingPin == pinDP)
        currentBlinkingPin = pinC;
      joystickMovedXState1 = !joystickMovedXState1;
    }
    else if (right) {
      if (currentBlinkingPin == pinA)
        currentBlinkingPin = pinB;
      else if (currentBlinkingPin == pinB) {
      } else if (currentBlinkingPin == pinC)
        currentBlinkingPin = pinDP;
      else if (currentBlinkingPin == pinD)
        currentBlinkingPin = pinC;
      else if (currentBlinkingPin == pinE)
        currentBlinkingPin = pinC;
      else if (currentBlinkingPin == pinF)
        currentBlinkingPin = pinB;
      else if (currentBlinkingPin == pinG) {
      } 
      else if (currentBlinkingPin == pinDP) {
      }
      joystickMovedXState1 = !joystickMovedXState1;
    } 
    else if (xMoving)
      joystickMovedXState1 = LOW;
  }
}

void readXY(){
  yValue = analogRead(pinX);
  xValue = analogRead(pinY);
}

void changeStateByPressDuration(){
  swState = digitalRead(pinSw);
  if (!swState == lastSwState)
    lastDebounceTime = millis();
  timePassedSinceDebounce = millis() - lastDebounceTime;
  if (timePassedSinceDebounce > DEBOUNCE_DELAY) {
    if (!swState == switchState) {
      switchState = swState;
      if (switchState == LOW)
        pressedTime = millis();
      else
        releasedTime = millis();
      long pressingDuration = releasedTime - pressedTime;
      if (pressingDuration > LONG_PRESS_TIME && stateIndex == 1)
        backToInitialState();
      else if (pressingDuration > NORMAL_AKA_SHORT_PRESS_TIME) {
        numberOfPresses = numberOfPresses + 1;
        if (numberOfPresses % 2 == 1) {
          notBlinkingPinState = HIGH;
          stateIndex = 2;
        } else {
          notBlinkingPinState = LOW;
          stateIndex = 1;
        }
      }
    }
  }
  lastSwState = swState;
}

void slideXAxisToTurnLedOnOff() {
  bool rightState2 = (xValue > MAX_THRESHOLD && joystickMovedXState2 == LOW);
  bool leftState2 = (xValue < MIN_THRESHOLD && joystickMovedXState2 == LOW);
  bool xMovingState2 = (joystickMovedXState2 == HIGH && xValue < MAX_THRESHOLD && xValue > MIN_THRESHOLD);
  if (stateIndex == 2) {
    if (leftState2) {
      currentBlinkingPinState = HIGH;
      if (currentBlinkingPin == pinA)
        pinAState = HIGH;
      else if (currentBlinkingPin == pinB)
        pinBState = HIGH;
      else if (currentBlinkingPin == pinC)
        pinCState = HIGH;
      else if (currentBlinkingPin == pinD)
        pinDState = HIGH;
      else if (currentBlinkingPin == pinE)
        pinEState = HIGH;
      else if (currentBlinkingPin == pinF)
        pinFState = HIGH;
      else if (currentBlinkingPin == pinG)
        pinGState = HIGH;
      else if (currentBlinkingPin == pinDP)
        pinDPState = HIGH;
      joystickMovedXState2 = LOW;
    } 
    else if (rightState2) {
      currentBlinkingPinState = LOW;
      if (currentBlinkingPin == pinA)
        pinAState = LOW;
      else if (currentBlinkingPin == pinB)
        pinBState = LOW;
      else if (currentBlinkingPin == pinC)
        pinCState = LOW;
      else if (currentBlinkingPin == pinD)
        pinDState = LOW;
      else if (currentBlinkingPin == pinE)
        pinEState = LOW;
      else if (currentBlinkingPin == pinF)
        pinFState = LOW;
      else if (currentBlinkingPin == pinG)
        pinGState = LOW;
      else if (currentBlinkingPin == pinDP)
        pinDPState = LOW;
      joystickMovedXState2 = LOW;
    } 
    else if (xMovingState2)
      joystickMovedXState2 = LOW;
  }
}

void loop() {
  readXY();
  outputLeds();
  state1Blinking();
  slideXAxisToTurnLedOnOff();
  changeStateByPressDuration();
}