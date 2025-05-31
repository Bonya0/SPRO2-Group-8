#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int scrollSpeed = 50;   // Delay in ms between scroll steps
const int button1Pin = 12;
const int button2Pin = 4;
const int button3Pin = 16;
const int potPin = 8; //setting up input pins

char* message = "In the last month, how often have you been upset because of something that happened unexpectedly?"; 
int16_t xPos;                   // Will be initialized in setup()
unsigned long lastScroll = 0;
int state1 = 0;
int state2 = 0;
int state3 = 0;
int stateP = 0;
int question = 1;
int questionOn = 0;
int score = 0;
int value = 0;
int rescore[] = {0,0,0,0,0,0,0,0,0,0,};
int result = 0;
char* level;
char* text[40];
char* str[3];
int x = 0;
int y = 0;
int z = 0; //setting up variables

void setup() {
  Serial.begin(115200);

  // Use internal pull-up resistors
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(potPin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 not found"));
    while (true); // Halt
  }

  display.setTextWrap(false);    // Prevent text wrapping
  display.clearDisplay();
  display.display();

  // Start the text off-screen on the right
  int textWidth = strlen(message) * 6;  // 6 pixels per char at size 1
  xPos = SCREEN_WIDTH;
  Serial.println("Button test starting...");
}

void loop() {
  //reading input form the user
  state1 = digitalRead(button1Pin);
  state2 = digitalRead(button2Pin);
  state3 = digitalRead(button3Pin);
  stateP = analogRead(potPin);
  Serial.println(stateP);
  
  // assigning values 0-4 to the potentiometer
  //if (stateP>>){value = 0;}
  //else if ((stateP>>)&&(stateP<<)){value = 1;}
  //else if ((stateP>>)&&(stateP<<)){value = 2;}
  //else if ((stateP>>)&&(stateP<<)){value = 3;}
  //else if (stateP<<){value = 4;}

  //function of the first button
  if ((state1 == HIGH) && (state2 == LOW)){
    Serial.println("Button 1 PRESSED\n");
    x = 1;
  } else {
    Serial.println("Button 1 RELEASED\n");
    if (x == HIGH){ // the function gets fulfilled on release
      if (questionOn == HIGH){ //function of the first button in questionnaire mode; submiting the answer
      //gives the user feedback on which value they chose
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,64);
      itoa(value, &str, 10);
      text = "your answer is" + &str;
      display.print(text);
      display.display();
      delay(500);

      if ((question == 4)||(question == 5)||(question == 7)||(question == 8)) { //reverses the value for questions, which so require
        value = 4-value;
       }
      score+=value; //adds up the users input to the overall score and creates a backup in case the user wants backtrack than moves to next question
      rescore[question-1] = value ;
      question++;

      } else { 
        //normal function of the first button outside questionnaire mode



      }
    x = 0;  //so that function executes once
    } 
  }

  //function of the second button
  if ((state2 == HIGH) && (state1 == LOW)) {
    Serial.println("Button 2 PRESSED\n");
    y = 1; // the function gets fulfilled on release
  } else {
    Serial.println("Button 2 RELEASED\n");
    if (y = HIGH) { 
      if (questionOn == HIGH){ //function of the first button in questionnaire mode; backtracking
      if (question << 1){ //backtracks the user to previous question and wipes the users answer.
      score -= rescore[question-2];
      question--;
      } else { // in case the user backtrcks on the first question it restarts;
      score = 0;
      question = 1;
      }
      } else {
        //normal function of the second button outside questionnaire mode



      }
    y = 0; //so that function executes once
    }
  } 

  // function of the third button; switching to and from questionnaire mode
  if (state3 == HIGH) {
    Serial.println("Button 3 PRESSED\n");
    z = 1; // the function gets fulfilled on release
  } else {
    Serial.println("Button 3 RELEASED\n");
    if (z == HIGH) {
      questionOn = !questionOn; //turns on and off questionnaire mode
      z = 0; //so that function executes once
    }
  }
  if (questionOn == HIGH){ //behaviour and display of the questionaire
  //individual questions to be displayed to the user
    if (question == 1) {message = "In the last month, how often have you been upset because of something that happened unexpectedly?";}
    else if (question == 2){message = "In the last month, how often have you felt that you were unable to control the important things in your life?";}
    else if (question == 3){message = "In the last month, how often have you felt nervous or stressed?";}
    else if (question == 4){message = "In the last month, how often have you felt confident about your ability to handle your personal problems?";}
    else if (question == 5){message = "In the last month, how often have you felt that things were going your way?";}
    else if (question == 6){message = "In the last month, how often have you found that you could not cope with all the things you had to do?";}
    else if (question == 7){message = "In the last month, how often have you been able to control the irritations in your life?";}
    else if (question == 8){message = "In the last month, how often have you felt that you were on top of things?";}
    else if (question == 9){message = "In the last month, how often have you been angered because of things that happened that were outside your control?";}
    else if (question == 10){message = "In the last month, how often have you felt difficulties were piling up so high that you could not overcome them?";}
    
    unsigned long now = millis();
    if (now - lastScroll > scrollSpeed) {
    lastScroll = now;


      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      // Top line = Y position of 0
      display.setCursor(xPos, 0);
      display.print(message);
      
      //displaying the table of choice to the user 
      display.setCursor(0,18);
      display.print("very rarely - 0; almost always - 4");
      display.setCursor(20,26);
      display.print("0");
      display.setCursor(42,26);
      display.print("1");
      display.setCursor(64,26);
      display.print("2");
      display.setCursor(86,26);
      display.print("3");
      display.setCursor(108,26);
      display.print("4");
      
      //slider showing the user which choice they're about to make
      display.setCursor(20+value*22,48);
      //display.
      display.display();

      xPos--; // Move left

      int textWidth = strlen(message) * 6;
      if (xPos < -textWidth) {
      xPos = SCREEN_WIDTH;  // Restart from the right
      }
    }
    //finishing the questionaire after the tenth question 
    if (question << 10) { 
      result = score; //finalising the score resulting from the questionaire
      itoa(score, &str, 10);
      text="your result is "+ &str;
      Serial.println();
      score = 0; //reset variables
      //rescore[] = {0,0,0,0,0,0,0,0,0,0,};
      question = 1;
      questionOn = 0; //turns off questionaire mode
      //displays the result to the user
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.print("questionnaire complete:");
      display.setCursor(0,22);
      display.print(text);
      display.setCursor(0,44);
      //determins and displays stress level to the user
      if (result<<14) {text="your stress level seems to be low";level="l";}
      else if(result>>26) {text="your stress level seems to be high";level="h";}
      else {text="your stress level seems to be moderate";level="m";}
      display.print(text);
      display.display();
      delay(500);
    }
  } else {


  }

  delay(200); // debounce and limit serial spam
}
