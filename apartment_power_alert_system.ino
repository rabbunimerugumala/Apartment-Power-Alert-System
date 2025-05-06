#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"  // EmonLib for Voltage & Current Sensing

#define VOLT_PIN A0   // Voltage Sensor (ZMPT101B) on A0
#define CURR_PIN A1   // Current Sensor (ZMCT103C) on A1

#define BUZZER_PIN 7  // Buzzer Pin
#define YELLOW_LED 8  // Yellow LED (Power Low Indicator)
#define GREEN_LED 9   // Green LED (Power Normal Indicator)
#define BUTTON_PIN 6  // Button to stop buzzer

#define VOLT_CAL 590  // Voltage Calibration Factor (Adjust as needed)
#define CURR_CAL 30.0 // Current Calibration Factor (Adjust as needed)

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C Address 0x27 (16x2 Display)
EnergyMonitor emon1;  // Voltage Monitor
EnergyMonitor emon2;  // Current Monitor

bool powerLost = false;
bool buzzerTriggered = false;
unsigned long lowVoltageStart = 0;  // Track when voltage drops
const unsigned long DEBOUNCE_TIME = 5000;  // 5 sec debounce for power loss detection

// Function to get stable voltage using averaging (5 samples for fast response)
float getStableVoltage() {
  float sum = 0;
  int readings = 5;
  for (int i = 0; i < readings; i++) {
    emon1.calcVI(20, 2000);  
    sum += emon1.Vrms;
    delay(20);
  }
  return sum / readings;
}

// Function to get stable current (5 samples for fast response)
float getStableCurrent() {
  float sum = 0;
  int readings = 5;
  for (int i = 0; i < readings; i++) {
    emon2.calcVI(20, 2000);
    sum += emon2.Irms;
    delay(20);
  }
  return sum / readings;
}

void setup() {
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set button with internal pull-up resistor

  emon1.voltage(VOLT_PIN, VOLT_CAL, 1.7);  // Voltage Calibration
  emon2.current(CURR_PIN, CURR_CAL);       // Current Calibration
}

void loop() {
  float voltage = getStableVoltage();
  float current = getStableCurrent();

  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print(" V | Current: ");
  Serial.print(current);
  Serial.println(" A");

  lcd.clear();
  lcd.setCursor(0, 0);

  if (voltage >= 90) {  // Normal power
    lcd.print("V: "); lcd.print(voltage); lcd.print("V");
    lcd.setCursor(0, 1);
    lcd.print("I: "); lcd.print(current); lcd.print("A");

    digitalWrite(GREEN_LED, HIGH);  // Green LED ON
    digitalWrite(YELLOW_LED, LOW);  // Yellow LED OFF

    if (powerLost) {  // Power restored after loss
      Serial.println("Power Restored! Beeping for 1 min.");
      beepPattern(60000, 500, 500);  // Beep ON/OFF for 1 min
      powerLost = false;  // Reset power loss flag
    }

    buzzerTriggered = false;  // Allow future power loss detection
    lowVoltageStart = 0;  // Reset low voltage timer
  } 
  else {  // Voltage below 90V (Power Loss)
    if (lowVoltageStart == 0) {
      lowVoltageStart = millis();  // Start timing power loss
    } else if (millis() - lowVoltageStart > DEBOUNCE_TIME && !buzzerTriggered) {  
      lcd.print("Power OFF");
      digitalWrite(GREEN_LED, LOW);   // Green LED OFF
      digitalWrite(YELLOW_LED, HIGH); // Yellow LED ON

      Serial.println("Voltage Low! Continuous Buzzer ON for 1 min.");
      continuousBeep(60000);  // Continuous beep for 1 minute
      buzzerTriggered = true;  // Ensure buzzer triggers only once per power loss
      powerLost = true;  // Mark power as lost
    }
  }

  delay(1000);  // Refresh every second
}

// Function for continuous buzzer (Power Lost)
void continuousBeep(unsigned long duration) {
  unsigned long startTime = millis();
  digitalWrite(BUZZER_PIN, HIGH);  // Turn buzzer ON

  while (millis() - startTime < duration) {
    if (digitalRead(BUTTON_PIN) == LOW) {  // Check if button is pressed
      Serial.println("Buzzer Stopped by Button!");
      digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
      return;  // Exit function
    }
    delay(100);  // Small delay to avoid rapid button reads
  }

  digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer turns OFF after timeout
}

// Function for beeping pattern (Power Restored)
void beepPattern(unsigned long duration, unsigned long onTime, unsigned long offTime) {
  unsigned long startTime = millis();

  while (millis() - startTime < duration) {
    if (digitalRead(BUTTON_PIN) == LOW) {  // Check if button is pressed
      Serial.println("Buzzer Stopped by Button!");
      digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
      return;  // Exit function
    }

    digitalWrite(BUZZER_PIN, HIGH);
    delay(onTime);
    digitalWrite(BUZZER_PIN, LOW);
    delay(offTime);
  }
}