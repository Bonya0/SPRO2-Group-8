#include "DFRobot_BloodOxygen_S.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define I2C_ADDRESS    0x57 

DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


int lastValidSPO2 = 0;
int lastValidBPM = 0;
float lastValidTemp = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 not found"));
    for (;;); 
  }

  Wire.begin(21, 22); 
  Wire.begin();  

  display.clearDisplay();

  display.setTextSize(1);   
  display.setTextColor(SSD1306_WHITE);

  if (!MAX30102.begin()) {
    Serial.println("MAX30102 init failed!");
    while (1);
  }


  MAX30102.sensorStartCollect(); 
}

void loop() {
  int analogVolts = analogReadMilliVolts(D2);
  long temperature = analogVolts / 10.0;


  MAX30102.getHeartbeatSPO2();

  int spo2 = MAX30102._sHeartbeatSPO2.SPO2;
  int bpm = MAX30102._sHeartbeatSPO2.Heartbeat;
  float temp = MAX30102.getTemperature_C();

  if (spo2 != -1){
    lastValidSPO2 = spo2;
  }
  if (bpm != -1){
    lastValidBPM = bpm;
  } 
  if (temp != -1) {
    lastValidTemp = temp;
  }

  if ((lastValidSPO2 > 0) && (lastValidBPM > 0) && (lastValidTemp > 0)){
    display.clearDisplay();

    display.setCursor(10, 0);   
    display.print("SPO2: ");
    display.print(lastValidSPO2);
    display.println("%");

    display.setCursor(10, 10);   
    display.print("Heart Rate: ");
    display.print(lastValidBPM);
    display.println(" bpm");

    display.setCursor(10, 20); 
    display.print("Board Temp: ");
    display.print(lastValidTemp);
    display.println(" C");

    display.setCursor(10, 45);   
    display.print("Ambient Temp: ");
    display.print(temperature);
    display.println(" C");

    display.display();

    delay(5000);
  }
  else{
    display.clearDisplay();

    display.setCursor(10, 0); 
    display.println("Initializing...");
    delay(5000);

    display.display();
  }
}