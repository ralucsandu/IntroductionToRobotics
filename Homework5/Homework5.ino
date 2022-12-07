#include <LiquidCrystal.h>
#include "LedControl.h"
#include <EEPROM.h>

#define LCD_BRIGHTNESS_ADDRESS 100
#define LCD_CONTRAST_ADDRESS 200
#define MTX_BRIGHTNESS_ADDRESS 300
#define BUZZER_SOUND_ADDRESS 400
#define HIGHSCORE_ADDRESS 800

//about stuff
const char aboutStuff[] = "                Pixel Eater by Raluca & Serban: https://github.com/ralucsandu, https://github.com/FiloteSerban                ";
const char howToPlay[] = "                Move the joystick to control the blinking pixel. Eat all of the non blinking pixels!                ";

//matrix pins
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

const byte matrixLedsOn = B11111111;

//buzzer pins
const byte buzzerPin = A2;
const int buzzerDuration = 20000;
#define NOTE_FS5 740

//lcd pins
const byte rs = 13;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte pinA = 3;
const byte pinV0 = 9;

//joystick pins
const byte pinSW = 2;
const byte pinX = A0;
const byte pinY = A1;

const int switchOff = 1;
int xValue = 0;
int yValue = 0;

int xValueGame = 0;
int yValueGame = 0;

int minThreshold = 300;
int maxThreshold = 800;
bool joyMovedy = false;
bool joyMovedx = false;
bool joyMoved = false;

//menu stuff
int menuPosition = 0;
int menuSettingsPosition = 0;

const int numberOfMenuOptions = 6;
const int numberOfMenuSettingsOptions = 5;

long highscore = 0;

unsigned int level = 1;

unsigned int lcdContrast = 7;
unsigned int lcdBrightness = 8;
unsigned int matrixBrightness = 0;
const int maxContrastBrightness = 255;

bool sound = true;

char menuStructure[numberOfMenuOptions][20] = {
  "   Game Menu  ",
  " Start Game",
  " HighScore",
  " Settings",
  " About",
  " How to play"
};

char menuSettingsStructure[numberOfMenuSettingsOptions][20] = {
  "Level",
  "LCD Contrast",
  "LCD Brightness",
  "Mtx Brightness",
  "Sound ON/OFF",
};

bool gameMenuSelected = true;
bool settingsMenuSelected = true;

unsigned long lastDebounce = 0;
unsigned long minReactionTime = 100;

const int matrixSize = 8;
const int invalidPosition = -1;
const int targetPosition = 7;
const int lcdLength = 16;

byte down[matrixSize] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100,
};

byte up[matrixSize] = {
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
};

byte full[matrixSize] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte heart[matrixSize] = {
  B00000,
  B11011,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

bool buttonDecider = 0;
bool buttonSettingsDecider = 0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int lives = 3;
int score = 0;
int timerLevel1 = 60;
int timerLevel2 = 45;
int timerLevel3 = 30;
int timerLevel4 = 20;
int timer = 0;
unsigned long sec = 0;
bool timeOK = true;
bool messageOK = true;

bool matrix[8][8];

int playerX = 0;
int playerY = 0;

void turnOnPixel(int posx, int posy) {
  matrix[posx][posy] = 1;
  lc.setLed(0, posx, posy, matrix[posx][posy]);
}

void turnOffPixel(int posx, int posy) {
  matrix[posx][posy] = 0;
  lc.setLed(0, posx, posy, matrix[posx][posy]);
}

void game() {
  turnOnPixel(playerX % matrixSize, playerY % matrixSize);
  turnOnPixel(targetPosition, targetPosition);
  int auxPlayerX = playerX;
  int auxPlayerY = playerY;
  xValueGame = analogRead(pinX);
  yValueGame = analogRead(pinY);

  if (yValueGame < minThreshold && joyMoved == false && xValueGame >= minThreshold && xValueGame <= maxThreshold) {
    playerX--;
    if (playerX == invalidPosition)
      playerX = matrixSize;
    joyMoved = true;
  }

  else if (yValueGame >= minThreshold && yValueGame <= maxThreshold && joyMoved == true && xValueGame >= minThreshold && xValueGame <= maxThreshold) {
    joyMoved = false;
  }

  else if (yValueGame > maxThreshold && joyMoved == false && xValueGame >= minThreshold && xValueGame <= maxThreshold) {
    playerX++;
    joyMoved = true;
  }

  else if (xValueGame < minThreshold && joyMoved == false && yValueGame >= minThreshold && yValueGame <= maxThreshold) {
    playerY--;
    if (playerY == invalidPosition)
      playerY = matrixSize;
    joyMoved = true;
  }

  else if (xValueGame >= minThreshold && xValueGame <= maxThreshold && joyMoved == true && yValueGame >= minThreshold && yValueGame <= maxThreshold) {
    joyMoved = false;
  }

  else if (xValueGame > maxThreshold && joyMoved == false && yValueGame >= minThreshold && yValueGame <= maxThreshold) {
    playerY++;
    joyMoved = true;
  }

  if (playerX % matrixSize == targetPosition && playerY % matrixSize == targetPosition) {
    score++;
    playerX = 0;
    playerY = 0;
  }

  turnOffPixel(auxPlayerX % matrixSize, auxPlayerY % matrixSize);
}

int backToMenu(bool &menu, bool &button) {
  xValue = analogRead(pinX);
  if (xValue < minThreshold && joyMovedx == false) {
    joyMovedx = true;
  } else if (xValue > minThreshold && xValue < maxThreshold && joyMovedx == true) {
    joyMovedx = false;
    lcd.clear();
    menu = true;
    button = 0;
    return 1;
  }
  return 0;
}

void showLetters(int printStart, int startLetter, char text[]) {
  lcd.setCursor(printStart, 1);
  for (int letter = startLetter; letter < startLetter + lcdLength; letter++) {
    lcd.print(text[letter]);
  }
  lcd.print(" ");
  delay(500);
}

int displayAbout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(6, 0);
  lcd.print("About");
  for (int letter = 0; letter <= strlen(aboutStuff) - lcdLength; letter++) {
    showLetters(0, letter, aboutStuff);
    if (backToMenu(gameMenuSelected, buttonDecider))
      return 0;
  }
}

int displayHowToPlay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(3, 0);
  lcd.print("How to play");

  for (int letter = 0; letter <= strlen(howToPlay) - lcdLength; letter++) {
    showLetters(0, letter, howToPlay);
    if (backToMenu(gameMenuSelected, buttonDecider))
      return 0;
  }
}

void menuScroller() {
  lcd.setCursor(15, 1);
  lcd.write(byte(0));
  lcd.setCursor(15, 0);
  lcd.write(byte(1));
  lcd.setCursor(0, 0);
  lcd.print(">");

  lcd.setCursor(1, 0);
  lcd.print(menuStructure[menuPosition % numberOfMenuOptions]);
  lcd.setCursor(0, 1);
  lcd.print(menuStructure[(menuPosition + 1) % numberOfMenuOptions]);

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false && menuPosition > 0) {
    menuPosition--;
    lcd.clear();
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    menuPosition++;
    lcd.clear();
    joyMovedy = true;
  }

  if (digitalRead(pinSW) == switchOff) {
    if (millis() - lastDebounce > minReactionTime && buttonDecider != 0) {
      gameMenuSelected = false;
    }
    lastDebounce = millis();
    buttonDecider = 1;
  }
}

void settingsScroller() {
  lcd.setCursor(15, 1);
  lcd.write(byte(0));
  lcd.setCursor(15, 0);
  lcd.write(byte(1));
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.setCursor(0, 0);
  lcd.print("<");

  lcd.setCursor(1, 0);
  lcd.print("   Settings");
  lcd.setCursor(1, 1);
  lcd.print(menuSettingsStructure[menuSettingsPosition % numberOfMenuSettingsOptions]);

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false && menuSettingsPosition > 0) {
    menuSettingsPosition--;
    lcd.clear();
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    menuSettingsPosition++;
    lcd.clear();
    joyMovedy = true;
  }

  if (digitalRead(pinSW) == switchOff) {
    if (millis() - lastDebounce > minReactionTime && buttonSettingsDecider != 0) {
      settingsMenuSelected = false;
    }
    lastDebounce = millis();
    buttonSettingsDecider = 1;
  }
  backToMenu(gameMenuSelected, buttonDecider);
}

void showHighscore() {
  highscore = readStringFromEEPROM(HIGHSCORE_ADDRESS).toInt();
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print("   Highscore   ");
  lcd.setCursor(0, 1);
  lcd.print("        ");
  lcd.print(highscore);
  lcd.setCursor(15, 1);
  lcd.print(" ");

  backToMenu(gameMenuSelected, buttonDecider);
}

void setLevel() {
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print("     Level     ");
  lcd.setCursor(0, 1);
  lcd.print("        ");
  lcd.print(level);
  lcd.setCursor(15, 1);
  lcd.print(" ");
  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false) {
    if (level < 4)
      level++;
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    if (level > 1)
      level--;
    joyMovedy = true;
  }
  backToMenu(settingsMenuSelected, buttonSettingsDecider);
}

void setContrast() {
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print("    Contrast   ");
  lcd.setCursor(0, 1);
  lcdContrast = readStringFromEEPROM(LCD_CONTRAST_ADDRESS).toInt();
  for (int i = 0; i < lcdContrast; ++i)
    lcd.write(byte(2));

  for (int i = lcdContrast; i < lcdLength; ++i)
    lcd.print(" ");

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false) {
    if (lcdContrast < lcdLength)
      lcdContrast++;
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    if (lcdContrast > 0)
      lcdContrast--;
    joyMovedy = true;
  }
  updateStringToEEPROM(LCD_CONTRAST_ADDRESS, String(lcdContrast));
  analogWrite(pinV0, (maxContrastBrightness / lcdLength) * lcdContrast);
  backToMenu(settingsMenuSelected, buttonSettingsDecider);
}

void setLcdBrightness() {
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print(" LcdBrightness  ");
  lcd.setCursor(0, 1);
  lcdBrightness = readStringFromEEPROM(LCD_BRIGHTNESS_ADDRESS).toInt();
  for (int i = 0; i < lcdBrightness; ++i)
    lcd.write(byte(2));

  for (int i = lcdBrightness; i < lcdLength; ++i)
    lcd.print(" ");

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false) {
    if (lcdBrightness < lcdLength)
      lcdBrightness++;
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    if (lcdBrightness > 0)
      lcdBrightness--;
    joyMovedy = true;
  }
  updateStringToEEPROM(LCD_BRIGHTNESS_ADDRESS, String(lcdBrightness));
  analogWrite(pinA, (maxContrastBrightness / lcdLength) * lcdBrightness);
  backToMenu(settingsMenuSelected, buttonSettingsDecider);
}

void setMatrixBrightness() {
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print(" MtxBrightness  ");
  lcd.setCursor(0, 1);
  matrixBrightness = readStringFromEEPROM(MTX_BRIGHTNESS_ADDRESS).toInt();
  for (int i = 0; i < matrixBrightness; ++i)
    lcd.write(byte(2));

  for (int i = matrixBrightness; i < lcdLength; ++i)
    lcd.print(" ");

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false) {
    if (matrixBrightness < lcdLength)
      matrixBrightness++;
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    if (matrixBrightness > 0)
      matrixBrightness--;
    joyMovedy = true;
  }
  updateStringToEEPROM(MTX_BRIGHTNESS_ADDRESS, String(matrixBrightness));
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  for (int i = 0; i < matrixSize; ++i)
    lc.setRow(0, i, matrixLedsOn);
  if(backToMenu(settingsMenuSelected, buttonSettingsDecider))
  {
    for(int row = 0; row < matrixSize; ++row)
      for(int col = 0; col < matrixSize; ++col)
        turnOffPixel(row,col);
  }
}

void setSound() {
  lcd.setCursor(0, 0);
  lcd.print("<");
  lcd.setCursor(1, 0);
  lcd.print("     Sound     ");
  lcd.setCursor(0, 1);
  lcd.print("       ");
  sound = EEPROM.read(BUZZER_SOUND_ADDRESS);
  if (sound) {
    lcd.print("ON");
    lcd.print("       ");
    tone(buzzerPin, NOTE_FS5, buzzerDuration);
  } else {
    lcd.print("OFF");
    lcd.print("      ");
    noTone(buzzerPin);
  }

  yValue = analogRead(pinY);

  if (yValue < minThreshold && joyMovedy == false) {
    sound = !sound;
    joyMovedy = true;
  }

  else if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedy == true) {
    joyMovedy = false;
  }

  else if (yValue > maxThreshold && joyMovedy == false) {
    sound = !sound;
    joyMovedy = true;
  }
  EEPROM.update(BUZZER_SOUND_ADDRESS, sound);
  backToMenu(settingsMenuSelected, buttonSettingsDecider);
}

void StartGame() {
  game();
  if (messageOK) {
    messageOK = false;
    lcd.setCursor(0, 0);
    lcd.print("Hello! ^.^   ");
    lcd.setCursor(0, 1);
    lcd.print("be ready   ");
    delay(3000);
  }
  lcd.setCursor(0, 0);
  for (int i = 0; i < lives; ++i)
    lcd.write(byte(3));

  lcd.setCursor(3, 0);
  lcd.print("  Score:");
  lcd.print(score);
  lcd.setCursor(15, 0);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print("Time:");

  if (timeOK) {
    sec = millis();
    timeOK = false;
  }

  if (millis() - sec >= 1000) {
    timer++;
    timeOK = true;
  }

  if (level == 1)
    lcd.print(timerLevel1 - timer);
  else if (level == 2)
    lcd.print(timerLevel2 - timer);
  else if (level == 3)
    lcd.print(timerLevel3 - timer);
  else if (level == 4)
    lcd.print(timerLevel4 - timer);

  lcd.setCursor(7, 1);
  lcd.print("         ");
  if ((timerLevel1 - timer <= 9 && level == 1) || (timerLevel2 - timer <= 9 && level == 2) || (timerLevel3 - timer <= 9 && level == 3) || (timerLevel4 - timer <= 9 && level == 4)) {
    lcd.setCursor(6, 1);
    lcd.print(" ");
  }
  if ((timerLevel1 - timer <= 0 && level == 1) || (timerLevel2 - timer <= 0 && level == 2) || (timerLevel3 - timer <= 0 && level == 3) || (timerLevel4 - timer <= 0 && level == 4)) {
    lcd.clear();
    gameMenuSelected = true;
    buttonDecider = 0;
    if (highscore < score)
      highscore = score;
    score = 0;
    updateStringToEEPROM(HIGHSCORE_ADDRESS, String(highscore));
    turnOffPixel(7, 7);
    timer = 0;
    messageOK = true;
  }
}

void updateStringToEEPROM(int address, const String &strToWrite) {
  int len = strToWrite.length();
  //erasing the value stored at the address before
  for (int i = address; i < address + len; ++i) {
    EEPROM.update(i, 0);
  }
  //updating in EEPROM
  EEPROM.update(address, len);
  for (int i = 0; i < len; i++) {
    EEPROM.update(address + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int address) {
  //reading the length of the message from EEPROM
  int newStrLen = EEPROM.read(address);
  char data[newStrLen];
  //reading the string from EEPROM
  for (int i = 0; i < newStrLen; i++) {
    data[i] = EEPROM.read(address + 1 + i);
  }
  return String(data);
}

void setup() {
  lc.shutdown(0, false);
  lcd.begin(16, 2);
  lcd.createChar(0, down);
  lcd.createChar(1, up);
  lcd.createChar(2, full);
  lcd.createChar(3, heart);
  analogWrite(3, 40);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinV0, OUTPUT);
  lcdBrightness = readStringFromEEPROM(LCD_BRIGHTNESS_ADDRESS).toInt();
  lcdContrast = readStringFromEEPROM(LCD_CONTRAST_ADDRESS).toInt();
  matrixBrightness = readStringFromEEPROM(MTX_BRIGHTNESS_ADDRESS).toInt();
  sound = EEPROM.read(BUZZER_SOUND_ADDRESS);
  analogWrite(pinA, (maxContrastBrightness / lcdLength) * lcdBrightness);
  analogWrite(pinV0, (maxContrastBrightness / lcdLength) * lcdContrast);
  lc.setIntensity(0, matrixBrightness);
  setSound();
  Serial.begin(9600);
}

void loop() {

  if (gameMenuSelected == true)
    menuScroller();

  if (gameMenuSelected == false) {

    if (menuPosition % numberOfMenuOptions == 0)
      gameMenuSelected = true;
    else if (menuPosition % numberOfMenuOptions == 1) {
      StartGame();
    } else if (menuPosition % numberOfMenuOptions == 2)
      showHighscore();

    else if (menuPosition % numberOfMenuOptions == 3) {
      if (settingsMenuSelected == true)
        settingsScroller();
      else {
        if (menuSettingsPosition % numberOfMenuSettingsOptions == 0)
          setLevel();
        else if (menuSettingsPosition % numberOfMenuSettingsOptions == 1)
          setContrast();
        else if (menuSettingsPosition % numberOfMenuSettingsOptions == 2)
          setLcdBrightness();
        else if (menuSettingsPosition % numberOfMenuSettingsOptions == 3)
          setMatrixBrightness();
        else if (menuSettingsPosition % numberOfMenuSettingsOptions == 4)
          setSound();
      }
    } else if (menuPosition % numberOfMenuOptions == 4)
      displayAbout();
    else if (menuPosition % numberOfMenuOptions == 5)
      displayHowToPlay();
  }
}
