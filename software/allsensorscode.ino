#include "DFRobot_BloodOxygen_S.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define I2C_ADDRESS    0x57  // MAX30102 default I2C address

DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address
    Serial.println(F("SSD1306 not found"));
    for (;;); // Halt execution
  }

  // Enable Internal Pull-ups (though external ones are recommended)
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);

  Wire.begin();  // Initialize I2C for Arduino Nano (A4/A5)

  // Clear the buffer
  display.clearDisplay();

  // Set text properties
  display.setTextSize(1);      // Text size (1 = small, 2 = medium, etc.)
  display.setTextColor(SSD1306_WHITE); // White text

  MAX30102.sensorStartCollect(); //start measuring
}

void loop() {
  int analogVolts = analogReadMilliVolts(D2);
  long temperature = analogVolts / 10.0;


  MAX30102.getHeartbeatSPO2();

  display.clearDisplay();

  display.setCursor(10, 0);   // Set cursor position
  display.print("SPO2: ");
  display.print(MAX30102._sHeartbeatSPO2.SPO2);
  display.println("%");

  display.setCursor(10, 10);   // Set cursor position
  display.print("Heart Rate: ");
  display.print(MAX30102._sHeartbeatSPO2.Heartbeat);
  display.println(" bpm");

  display.setCursor(10, 20);   // Set cursor position
  display.print("Board Temperature: ");
  display.print(MAX30102.getTemperature_C());
  display.println(" C");

  display.setCursor(10, 45);   // Set cursor position
  display.print("Ambient Temperature: ");
  display.print(temperature);
  display.println(" C");

  display.display();

  delay(5000);  // Sensor updates every 4 seconds
}