#include "DHT.h"
#include <IRremote.hpp>
#include <LiquidCrystal.h>

#define Type DHT11

const int IRpin = 2;
const int sensePin = 6;
const int speedPin = 5;
const int dir1 = 4;
const int dir2 = 3;

DHT HT(sensePin, Type);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

float tempC;
int motorSpeed = 150;
bool manualOn = false; 

unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 400; // ms

void updateLCD(String status, float temp);

void setup() 
{
  lcd.begin(16, 2);
  HT.begin();
  IrReceiver.begin(IRpin, ENABLE_LED_FEEDBACK);
  
  pinMode(speedPin, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);

  // Set direction once
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, LOW);
}

void loop() 
{
  // Read IR Remote
  if (IrReceiver.decode())
  { 
    // Ignore repeat signals
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    unsigned long now = millis();

    // Debounce
    if (now - lastPressTime <= debounceDelay) {
      IrReceiver.resume();
      return;
    }
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;

    // On/Off button
    if (receivedCode == 3125149440) {
      manualOn = !manualOn;
      lastPressTime = now;
    }

    IrReceiver.resume();
  }

  // Read Temperature
  tempC = HT.readTemperature();

  // Turn on fan if temp is greater than 25C or ON button is pressed
  if (manualOn) 
  {
    analogWrite(speedPin, motorSpeed);
    updateLCD("Manual ON", tempC);
  } 
  else if (tempC > 25.0)
  {
    analogWrite(speedPin, motorSpeed);
    updateLCD("Temp AUTO", tempC);
  }
  else 
  {
      analogWrite(speedPin, 0);
      updateLCD("Off", tempC);
  }
  delay(200); 
}

void updateLCD(String status, float temp) {
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("C  "); // Extra spaces clear old digits
  lcd.setCursor(0, 1);
  lcd.print("Fan: ");
  lcd.print(status);
  lcd.print("      "); 
}