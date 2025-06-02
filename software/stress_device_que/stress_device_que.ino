#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int scrollSpeed = 50;   // Delay in ms between scroll steps
const int button1Pin = 12;
const int button2Pin = 4;
const int button3Pin = 16;
const int potPin = 8; //setting up input pins

char message[256];
int16_t xPos;
unsigned long lastScroll = 0;
int state1 = 0;
int state2 = 0;
int state3 = 0;
int stateP = 0;
int question = 1;
int questionOn = 0;
int score = 0;
int value = 0;
int rescore[10] = {0};
int result = 0;

char level[20];
char text[64];
char str[10];
int x = 0, y = 0, z = 0;

void setup() {
  Serial.begin(115200);

  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(potPin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 not found"));
    while (true);
  }

  display.setTextWrap(false);
  display.clearDisplay();
  display.display();

  strcpy(message, "In the last month, how often have you been upset because of something that happened unexpectedly?");
  xPos = SCREEN_WIDTH;

  Serial.println("Button test starting...");
}

void loop() {
  state1 = digitalRead(button1Pin);
  state2 = digitalRead(button2Pin);
  state3 = digitalRead(button3Pin);
  stateP = analogRead(potPin);

  value = map(stateP, 0, 4095, 0, 4);  // maps pot to 0–4

  // Button 1: Submit answer
  if (state1 == HIGH && state2 == LOW) {
    x = 1;
  } else {
    if (x == HIGH) {
      if (questionOn == HIGH) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 64);
        itoa(value, text, 10);
        display.print("your answer is ");
        display.print(text);
        display.display();
        delay(500);

        if (question == 4 || question == 5 || question == 7 || question == 8) {
          value = 4 - value;
        }

        score += value;
        rescore[question - 1] = value;
        question++;
      }
      x = 0;
    }
  }

  // Button 2: Go back one question
  if (state2 == HIGH && state1 == LOW) {
    y = 1;
  } else {
    if (y == HIGH) {
      if (questionOn == HIGH) {
        if (question > 1) {
          score -= rescore[question - 2];
          question--;
        } else {
          score = 0;
          question = 1;
        }
      }
      y = 0;
    }
  }

  // Button 3: Toggle questionnaire mode
  if (state3 == HIGH) {
    z = 1;
  } else {
    if (z == HIGH) {
      questionOn = !questionOn;
      z = 0;
    }
  }

  if (questionOn == HIGH) {
    switch (question) {
      case 1:
        strcpy(message, "In the last month, how often have you been upset because of something that happened unexpectedly?");
        break;
      case 2:
        strcpy(message, "In the last month, how often have you felt that you were unable to control the important things in your life?");
        break;
      case 3:
        strcpy(message, "In the last month, how often have you felt nervous or stressed?");
        break;
      case 4:
        strcpy(message, "In the last month, how often have you felt confident about your ability to handle your personal problems?");
        break;
      case 5:
        strcpy(message, "In the last month, how often have you felt that things were going your way?");
        break;
      case 6:
        strcpy(message, "In the last month, how often have you found that you could not cope with all the things you had to do?");
        break;
      case 7:
        strcpy(message, "In the last month, how often have you been able to control the irritations in your life?");
        break;
      case 8:
        strcpy(message, "In the last month, how often have you felt that you were on top of things?");
        break;
      case 9:
        strcpy(message, "In the last month, how often have you been angered because of things that happened that were outside your control?");
        break;
      case 10:
        strcpy(message, "In the last month, how often have you felt difficulties were piling up so high that you could not overcome them?");
        break;
    }

    unsigned long now = millis();
    if (now - lastScroll > scrollSpeed) {
      lastScroll = now;

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(xPos, 0);
      display.print(message);

      display.setCursor(0, 18);
      display.print("very rarely - 0; almost always - 4");

      for (int i = 0; i <= 4; i++) {
        display.setCursor(20 + i * 22, 26);
        display.print(i);
      }

      display.setCursor(20 + value * 22, 48);
      display.print("^");

      display.display();

      xPos--;
      int textWidth = strlen(message) * 6;
      if (xPos < -textWidth) {
        xPos = SCREEN_WIDTH;
      }
    }

    // After last question
    if (question > 10) {
      result = score;
      itoa(score, str, 10);
      strcpy(text, str);

      Serial.println("Your result is:");
      Serial.println(text);

      score = 0;
      for (int i = 0; i < 10; i++) rescore[i] = 0;
      question = 1;
      questionOn = 0;

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("Questionnaire complete:");
      display.setCursor(0, 22);
      display.print("Your result is ");
      display.print(text);
      display.setCursor(0, 44);

      if (result < 14) {
        strcpy(text, "Your stress level seems to be low");
        strcpy(level, "l");
      } else if (result > 26) {
        strcpy(text, "Your stress level seems to be high");
        strcpy(level, "h");
      } else {
        strcpy(text, "Your stress level seems to be moderate");
        strcpy(level, "m");
      }

      display.print(text);
      display.display();
      delay(500);
    }
  }

  delay(200);
}
