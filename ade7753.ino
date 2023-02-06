#include "ADE7753.h"
#include "RTC.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MyEEPROM.h"
#include "LCD.h"
#include <Ticker.h>

#define ADE ADE7753::getInstance()
#define LCD LCD::getInstance()
#define EEPROM MyEEPROM::getInstance()
#define RTC RtcDs1307::getInstance()

#define CS_PIN 16
#define EEPROM_SIZE 512

#define START_TIME_1_HOUR 9
#define START_TIME_1_MINUTES 30
#define STOP_TIME_1_HOUR 11
#define STOP_TIME_1_MINUTES 30

#define START_TIME_2_HOUR 17
#define START_TIME_2_MINUTES 0
#define STOP_TIME_2_HOUR 20
#define STOP_TIME_2_MINUTES 0

#define TIME_TO_SAVE_DATA 60      // seconds
#define TIME_TO_CALCULATE_WH 500  // ms

#define PRICE_HIGHLIGHT 2345  // price/kWh

LiquidCrystal_I2C mLCD(0x27, 16, 2);
Ticker mTickerCalculateWh;
Ticker mTickerSaveWh;

int page = 0;
long lastMillis = 0;
long lastMillis_ = 0;

void setup() {
  /* Init serial port for debug */
  Serial.begin(9600);
  /* Init peripheral */
  ADE->start(CS_PIN);
  // LCD->initLCD(&mLCD);
  EEPROM->initEEPROM(EEPROM_SIZE);
  /* Read data from eeprom */
  ADE->setWh(EEPROM->readEEPROM(0).toFloat());
  // RTC->initRTC();
  /* Init timer */
  mTickerCalculateWh.attach_ms(TIME_TO_CALCULATE_WH, calculateWh);
  mTickerSaveWh.attach(TIME_TO_SAVE_DATA, saveWh);
  /* Init LED pin */
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("System init done!");
  delay(50);
}

void loop() {
}

void calculateWh() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Serial.print("kWh: ");
  Serial.println(ADE->calculateWh());
  
  if (millis() - lastMillis_ >= 2000) {
    lastMillis_ = millis();
    if (page == 0) {
      page = 1;
    } else {
      page = 0;
    }
  }
}

void saveWh() {
  EEPROM->writeEEPROM(0, String(ADE->calculateWh()));
  Serial.println("Save data done!");
}
