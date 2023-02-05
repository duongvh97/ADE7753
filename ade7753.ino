#include "ADE7753.h"
#include "RTC.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EEPROM.h"

#define ADE ADE7753::getInstance()
#define LCD LCD::getInstance()
#define CS_PIN 16

LiquidCrystal_I2C mLCD(0x27, 16, 2);
float voltage, current, freq, power;
int page = 0;
long lastMillis = 0;

void setup() {
  Serial.begin(9600);
  ADE->start(CS_PIN);
  mLCD.begin();
  mLCD.clear();
  delay(10);
}

void loop() {
  voltage = ADE->getVoltage();
  current = ADE->getCurrent();
  freq = ADE->getFrequency();
  power = ADE->getPower();

  if(millis() - lastMillis >= 2000) {
    lastMillis = millis();
    if(page == 0) {
      page = 1;
    }
    else {
      page = 0;
    }
  }

  if(page == 0) {
    mLCD.clear();
    mLCD.setCursor(0, 0); mLCD.print("Vol: ");  
    mLCD.setCursor(5, 0); mLCD.print(voltage); mLCD.print(" V");
    mLCD.setCursor(0, 1); mLCD.print("Cur: ");  
    mLCD.setCursor(5, 1); mLCD.print(current); mLCD.print(" A");
  }
  else{
    mLCD.clear();
    mLCD.setCursor(0, 0); mLCD.print("Fre: ");  
    mLCD.setCursor(5, 0); mLCD.print(freq); mLCD.print(" Hz");
    mLCD.setCursor(0, 1); mLCD.print("Pow: ");  
    mLCD.setCursor(5, 1); mLCD.print(power); mLCD.print(" W");
  }
  delay(1000);
}
