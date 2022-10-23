const int bluePin = 11;
const int greenPin = 10;
const int redPin = 9;

const int potPinBlue = A0;
const int potPinGreen = A1;
const int potPinRed = A2;

const int maxAnalogValue = 1023;
const int minAnalogValue = 255;

int brightnessBlue = 0;
int brightnessGreen = 0;
int brightnessRed = 0;

int potValueBlue = 0;
int potValueGreen = 0;
int potValueRed = 0;

void setup() {
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  pinMode(potPinBlue, INPUT);
  pinMode(potPinGreen, INPUT);
  pinMode(potPinRed, INPUT);

  Serial.begin(9600);
}

void loop() {
  readColor(potPinRed, potPinGreen, potPinBlue);
  mapColor(brightnessRed, brightnessGreen, brightnessBlue);
  setColor(brightnessRed, brightnessGreen, brightnessBlue);
}

void readColor(int red, int green, int blue) {
  potValueBlue = analogRead(potPinBlue);
  potValueGreen = analogRead(potPinGreen);
  potValueRed = analogRead(potPinRed);
}

void setColor(int red, int green, int blue) {
  analogWrite(bluePin, brightnessBlue);
  analogWrite(greenPin, brightnessGreen);
  analogWrite(redPin, brightnessRed);
}

void mapColor(int red, int green, int blue) {
  brightnessBlue = map(potValueBlue, 0, maxAnalogValue, 0, minAnalogValue);
  brightnessGreen = map(potValueGreen, 0, maxAnalogValue, 0, minAnalogValue);
  brightnessRed = map(potValueRed, 0, maxAnalogValue, 0, minAnalogValue);
}