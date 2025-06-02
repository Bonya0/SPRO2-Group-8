/*
 * allsensorscode.ino
 *
 * Created: 1/6/2025
 * Author: Adam Boros
 * Contributor: Adam Kukucka
 *
 * Group: 8
 * GitHub URL: https://github.com/Bonya0/SPRO2-Group-8
 */ 


//including the used libraries
#include "DFRobot_BloodOxygen_S.h" //library for MAX30102 sensor
#include <Wire.h> //library for i2c
#include <Adafruit_GFX.h> //libraries for the oled display
#include <Adafruit_SSD1306.h>
#include "Grove_Temperature_And_Humidity_Sensor.h" //library for the dht11 sensor

#define SCREEN_WIDTH 128 //defining useful the for the oled display
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS    0x57

#define DHTTYPE DHT11 //defining the type of humidity sensor and its allocated pin
#define DHTPIN 17

#define ldr_pin A3 //defining the pin the LDR's using

DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS); //initializing the digital sensors
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

int analogVolts; //creating the variables
long temperature;
int spo2;
int bpm;
float temp;
float humidity;
float humidity_temp;

int ldr_value;
int ldr_scaled;

int lastValidSPO2 = 0;
int lastValidBPM = 0;
float lastValidTemp = 0;



const int button1Pin = 39;
const int button2Pin = 36;
const int button3Pin = 16;

int state1 = 0;
int state2 = 0;
int state3 = 0;

int menu_state = 0;

int potmeter = 0;
int q=0;
int answers[5];
int question_index = 0;


bool allQuestionsAnswered = false;
int score = 0;
int score_check = 0;

int stressed = false;

int16_t xPos;             
unsigned long lastScroll = 0;
const int scrollSpeed = 40;
const char* message[5] = {"In the last month, how often have you been upset because of something that happened unexpectedly?",
                          "In the last month, how often have you felt that you were unable to control the important things in your life?",
                          "In the last month, how often have you felt difficulties were piling up so high that you could not overcome them?",
                          "In the last month, how often have you found that you could not cope with all the things you had to do?",
                          "In the last month, how often have you been angered because of things that happened that were outside your control?"}; //array for questiinnare

void setup() {
  Serial.begin(115200); //setting up serial communication at 115200 bits/s
  delay(1000);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  //checking if the display is initialized
    Serial.println(F("SSD1306 not found"));
    for (;;); 
  }

  Wire.begin(21, 22); //setting up i2c
  Wire.begin();  
  dht.begin(); //starting the dht sensotr

   
  display.clearDisplay(); //setting for the display

  display.setTextSize(1);   
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false); 
  


  if (!MAX30102.begin()) { //checking if the max sensor is initialized
    Serial.println("MAX30102 init failed!");
    while (1);
  }


  MAX30102.sensorStartCollect(); //starting the max sensor

  pinMode(button1Pin, INPUT); //configuring the buttons
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);

}

void loop() {
  state1 = digitalRead(button1Pin); //reading buttons
  state2 = digitalRead(button2Pin);
  state3 = digitalRead(button3Pin);

  if (q!= 1){ //controlling the menu, if the user is in the questionnare mode, the program does not react the state changes
    if (state1 == HIGH) {
      menu_state = 1;
      q=1;
    } 

    if (state2 == HIGH) {
      menu_state = 2;
    } 

    if (state3 == HIGH) {
      menu_state = 3;
    } 
  }


  if (menu_state == 0){ //state for the menu which gives instructions
    display.clearDisplay(); //displaying on the display
    display.setCursor(0, 0);
    display.print("Stress sensor");

    display.setCursor(0, 10);
    display.print("by Group 8");

    display.setCursor(0, 30);
    display.print("Button 1: Questions");

    display.setCursor(0, 40);
    display.print("Button 2: Vitals");

    display.setCursor(0, 50);
    display.print("Button 3: Stressed?");

    display.display();
  }

  if (menu_state == 1) { //state for the questionnare
    unsigned long now = millis(); //checking how long the mc has been running
    potmeter = (analogRead(A2) / 1000); //reading the value of the potmeter and converting it to 0-4

    if (!allQuestionsAnswered && now - lastScroll > scrollSpeed) { //if the questions have been not answered and the message can be scrolled 
      lastScroll = now; //last scroll is equal to the time now (how ling the mc has been running)

      display.clearDisplay(); //displaying the given message, and scrolling it from the screen's width(Xpos)
      display.setCursor(xPos, 0);
      display.print(message[question_index]);
      xPos--;

      int textWidth = strlen(message[question_index]) * 6; //determining from where to scolll the message
      if (xPos < -textWidth) {
        xPos = SCREEN_WIDTH;
      }

      display.setCursor(64, 32); //printing the selected answer
      display.print(potmeter);

      display.setCursor(14, 50); //information
      display.print("Press B2 to save");

      display.display();
    }

    if (state2 == HIGH){ //if button 2 is pressed it saves the answer and goes to next state

      answers[question_index] = potmeter; //saving the answer
      question_index++; //going to the next question

      if (question_index >= 5) { //if all the questions have been answered the flag is set
        allQuestionsAnswered = true;
      } else {
        xPos = SCREEN_WIDTH; // Reset scroll
      }

      display.clearDisplay(); //printing and delay
      display.setCursor(0, 10);

      if (!allQuestionsAnswered) {
        display.print("Saved. Next question:");
      } 
      else {
        display.print("All answers saved!");
      }
      display.display();
      delay(1000);
    }

    if (allQuestionsAnswered) { //if all the questions have been answered it displays the final scores, and each answer
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Answers:");

      for (int i = 0; i < 5; i++) {
        display.setCursor(0, 10 + i * 10);
        display.print("Q");
        display.print(i + 1);
        display.print(": ");
        display.print(answers[i]);
        if (score_check == 0){
          score = score + answers[i];
        }
        
      }
      score_check = 1;

      display.setCursor(70, 28);
      display.print("Score: ");
      display.print(score);

      q=0; //the user can control the menu again
      display.display();
    }
  }

  if (menu_state == 2){ //state for the vital displaying
    analogVolts = analogReadMilliVolts(D2); //reading the LM35 and converting it to celsius
    temperature = analogVolts / 10.0;

    ldr_value = analogRead(ldr_pin); //reading the ldr and scaling it to 1-100
    ldr_scaled = ((ldr_value * 100)/4000)-2; 
    MAX30102.getHeartbeatSPO2();

    spo2 = MAX30102._sHeartbeatSPO2.SPO2; //getting all of the data from the sensors
    bpm = MAX30102._sHeartbeatSPO2.Heartbeat;
    temp = MAX30102.getTemperature_C();
    humidity = dht.readHumidity();
    humidity_temp = dht.readTemperature();

    if (spo2 != -1){ //checking if the max sensor sent garbage data or not, if it did the last correct data is displayed
    lastValidSPO2 = spo2;
    }
    if (bpm != -1){
      lastValidBPM = bpm;
    } 
    if (temp != -1) {
      lastValidTemp = temp;
    }

    if ((lastValidSPO2 > 0) && (lastValidBPM > 0) && (lastValidTemp > 0)){ //if all of the data is correct the vitals are printed
      display.clearDisplay();

      display.setCursor(0, 0);
      display.print("Light: ");
      display.print(ldr_scaled);
      display.print("%");

      display.setCursor(70, 0);   
      display.print("SPO2: ");
      display.print(lastValidSPO2);
      display.println("%");

      display.setCursor(10, 15);   
      display.print("Heart Rate: ");
      display.print(lastValidBPM);
      display.println(" bpm");

      display.setCursor(10, 25); 
      display.print("Board Temp: ");
      display.print(lastValidTemp);
      display.println(" C");

      display.setCursor(10, 35);
      display.print("Humidity: ");
      display.print(humidity);
      display.print(" %");

      display.setCursor(10, 45);
      display.print("Temperature: ");
      display.print(humidity_temp);

      display.setCursor(10, 55);   
      display.print("Ambient Temp: ");
      display.print(temperature);
      display.println(" C");





      display.display();

      delay(1000);
    }
    else{ //if not the system goes into initalization mode until theres correct data
      display.clearDisplay();

      display.setCursor(10, 0); 
      display.println("Initializing...");
      delay(1000);

      display.display();
    }
  }

  if (menu_state == 3){ //state for checking if you're stressed
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Are you stressed?");

    if (!allQuestionsAnswered){ //if the questions hadn't been answered it tells the user to do so
      display.setCursor(0, 20);
      display.print("Fill out the");
      display.setCursor(0, 30);
      display.print("questionnare first");
    }
    else{ //if the questions had been answered the program evaluates if the person is stressed with if statements

      if(score >= 10 && lastValidBPM > 130){
        stressed = true;
        display.setCursor(0, 20);
        display.print("You are stressed");
        display.setCursor(0, 30);
        display.print("Take a breath!");
      }

      if(score >= 10 && humidity > 65){
        stressed = true;
        display.setCursor(0, 20);
        display.print("You are stressed");
        display.setCursor(0, 30);
        display.print("Go outside!");
      }

      if(score >= 10 && lastValidTemp > 37.2){
        stressed = true;
        display.setCursor(0, 20);
        display.print("You are stressed");
        display.setCursor(0, 30);
        display.print("See a doctor!");
      }

      if(score >= 10 && lastValidSPO2 < 90){
        stressed = true;
        display.setCursor(0, 20);a
        display.print("You are stressed");
        display.setCursor(0, 30);
        display.print("Take a breath");
      }

      if(score >= 10 && ldr_scaled < 30){
        stressed = true;
        display.setCursor(0, 20);
        display.print("You are stressed");
        display.setCursor(0, 30);
        display.print("Take some Vitamin D");
      }

      if(!stressed){
        display.setCursor(0, 20);
        display.print("You are not stressed");
      }

    }

    display.display();
  }

}