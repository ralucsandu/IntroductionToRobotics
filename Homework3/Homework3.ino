#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define DP 7

#define NUMBER_OF_SEGMENTS 8

#define MIN_THRESHOLD 400
#define MAX_THRESHOLD 600

#define LONG_PRESS_TIME 1000
#define NORMAL_AKA_SHORT_PRESS_TIME 40
#define DEBOUNCE_DELAY 80
#define BLINKING_INTERVAL 400

bool commonAnode = false;
byte commonAnodeState = HIGH;

const int pinX = A0,
          pinY = A1;

const unsigned int pinSw = 2,
                   pinA = 4,
                   pinB = 5,
                   pinC = 6,
                   pinD = 7,
                   pinE = 8,
                   pinF = 9,
                   pinG = 10,
                   pinDP = 11;

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
             state = 1;

int yValue, xValue;

int segments[NUMBER_OF_SEGMENTS] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int segmentsStates[NUMBER_OF_SEGMENTS] = {
  pinAState, pinBState, pinCState, pinDState, pinEState, pinFState, pinGState, pinDPState
};

byte joystickMovedY = LOW,
     joystickMovedXState1 = LOW,
     joystickMovedXState2 = LOW;

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

int directions[8][4] = {
  { -1, G, F, B },
  { A, G, F, -1 },
  { G, D, E, DP },
  { G, -1, E, C },
  { G, D, -1, C },
  { A, G, -1, B },
  { A, D, -1, -1 },
  { -1, -1, C, -1 }
};

void setup() {
  for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i)
    pinMode(segments[i], OUTPUT);

  if (commonAnode == true)
    commonAnodeState = !commonAnodeState;

  pinMode(pinSw, INPUT_PULLUP);
  Serial.begin(9600);
}

void outputLeds() {
  for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i)
    if (currentBlinkingPin != segments[i])
      digitalWrite(segments[i], segmentsStates[i]);
  digitalWrite(currentBlinkingPin, currentBlinkingPinState);
}

void backToInitialState() {
  currentBlinkingPin = pinDP;
  for (int i = 0; i < 8; i++)
    segmentsStates[i] = LOW;
}

void readXY() {
  yValue = analogRead(pinX);
  xValue = analogRead(pinY);
}

void state1Blinking() {
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
  
  if (state == 1) {
    if (left) {
      if (directions[currentBlinkingPin - 4][2] != -1)
        currentBlinkingPin = segments[directions[currentBlinkingPin - 4][2]];
      joystickMovedXState1 = HIGH;
    } else if (right) {
      if (directions[currentBlinkingPin - 4][3] != -1)
        currentBlinkingPin = segments[directions[currentBlinkingPin - 4][3]];
      joystickMovedXState1 = HIGH;
    } else if (xMoving)
      joystickMovedXState1 = LOW;

    if (down) {
      if (directions[currentBlinkingPin - 4][1] != -1)
        currentBlinkingPin = segments[directions[currentBlinkingPin - 4][1]];
      joystickMovedY = HIGH;
    } else if (up) {
      if (directions[currentBlinkingPin - 4][0] != -1)
        currentBlinkingPin = segments[directions[currentBlinkingPin - 4][0]];
      joystickMovedY = HIGH;
    } else if (yMoving)
      joystickMovedY = LOW;
  }
}

void slideXAxisToTurnLedOnOff() {
  bool rightState2 = (xValue > MAX_THRESHOLD && joystickMovedXState2 == LOW);
  bool leftState2 = (xValue < MIN_THRESHOLD && joystickMovedXState2 == LOW);
  bool xMovingState2 = (joystickMovedXState2 == HIGH && xValue < MAX_THRESHOLD && xValue > MIN_THRESHOLD);

  if (state == 2) {
    if (leftState2) {
      currentBlinkingPinState = HIGH;
      for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i)
        if (currentBlinkingPin == segments[i]) {
          segmentsStates[i] = HIGH;
          break;
        }
      joystickMovedXState2 = LOW;
    } else if (rightState2) {
      currentBlinkingPinState = LOW;
      for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i)
        if (currentBlinkingPin == segments[i]) {
          segmentsStates[i] = LOW;
          break;
        }
      joystickMovedXState2 = LOW;
    } else if (xMovingState2)
      joystickMovedXState2 = LOW;
  }
}

void changeStateByPressDuration() {
  swState = digitalRead(pinSw);
  if (!swState == lastSwState)
    lastDebounceTime = millis();
  timePassedSinceDebounce = millis() - lastDebounceTime;
  if (timePassedSinceDebounce > DEBOUNCE_DELAY)
    if (!swState == switchState) {
      switchState = swState;
      if (switchState == LOW)
        pressedTime = millis();
      else
        releasedTime = millis();
      long pressingDuration = releasedTime - pressedTime;
      if (pressingDuration > LONG_PRESS_TIME && state == 1)
        backToInitialState();
      else if (pressingDuration > NORMAL_AKA_SHORT_PRESS_TIME) {
        numberOfPresses = numberOfPresses + 1;
        if (numberOfPresses % 2 == 1) {
          notBlinkingPinState = HIGH;
          state = 2;
        } else {
          notBlinkingPinState = LOW;
          state = 1;
        }
      }
    }
  lastSwState = swState;
}

void loop() {
  readXY();
  outputLeds();
  state1Blinking();
  slideXAxisToTurnLedOnOff();
  changeStateByPressDuration();
}