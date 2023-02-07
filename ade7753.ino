#include "ADE7753.h"
#include "RTC.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP_EEPROM.h>
#include "LCD.h"
#include <Ticker.h>

#define ADE ADE7753::getInstance()
#define LCD LCD::getInstance()
#define RTC RtcDs1307::getInstance()

#define CS_PIN 16
#define EEPROM_SIZE 32

#define START_TIME_1_HOUR 9
#define START_TIME_1_MINUTES 30
#define DURATION_TIME_1 (2.5 * 3600)
#define STOP_TIME_1_MINUTES 30

#define START_TIME_2_HOUR 17
#define START_TIME_2_MINUTES 0
#define DURATION_TIME_2 (3 * 3600)

#define TIME_TO_SAVE_DATA 120  // seconds
#define TIME_TO_CALCULATE_WH 1 // second
#define TIME_TO_SWITCH_SCREEN 2 // seconds

#define PRICE_PEAK 2345  // price for each kWh peak
#define PRICE_NOMARL 1234  // price for each kWh normal

LiquidCrystal_I2C mLCD(0x27, 16, 2);

Ticker mTickerCalculateWh;
Ticker mTickerSaveWh;
Ticker mTickerSwitchScreen;

int page = 0;
int counter, counter_;
long lastMillis = 0;
long lastMillis_ = 0;
float voltage, current, power, freq, Wh, totalMoney;
bool isDone = false;
bool isDone_ = false;

float bufferKwh;

int currentHour, currentMinutes, currentSecond;

void setup() {
  /* Init serial port for debug */
  Serial.begin(9600);
  /* Init peripheral */
  ADE->start(CS_PIN);
  LCD->initLCD(&mLCD);
  EEPROM.begin(EEPROM_SIZE);
  /* Read data from eeprom */
  // RTC->initRTC();
  /* Init timer */
  mTickerCalculateWh.attach(TIME_TO_CALCULATE_WH, calculateTotalMoney);
  mTickerSaveWh.attach(TIME_TO_SAVE_DATA, saveWh);
  mTickerSwitchScreen.attach(TIME_TO_SWITCH_SCREEN, switchScreen);
  /* Init LED pin */
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("System init done!");
  EEPROM.get(0, bufferKwh);
  ADE->setWh(bufferKwh);
  EEPROM.get(10, totalMoney);
  Serial.println("Get kWh from EEPROM: " + String(bufferKwh));
  Serial.println("Get totalMoney from EEPROM: " + String(totalMoney));
  delay(50);
}

void loop() {
}

void switchScreen() {
  freq = ADE->getFrequency();
  
  if (millis() - lastMillis_ >= 2000) {
    lastMillis_ = millis();
    if (page == 0) {
      page = 1;
      LCD->lcdClear();
      LCD->lcdPrint(0, 0, "Vol: ");  LCD->lcdPrint(0, 5, String(voltage) + " V");
      LCD->lcdPrint(1, 0, "Cur: ");  LCD->lcdPrint(1, 5, String(current) + " A");
    } 
    else if(page == 1) {
      page = 2;
      LCD->lcdClear();
      LCD->lcdPrint(0, 0, "Freq: ");  LCD->lcdPrint(0, 6, String(freq) + " Hz");
      LCD->lcdPrint(1, 0, "Power: ");  LCD->lcdPrint(1, 7, String(power) + " W");
    }
    else {
      page = 0;
      LCD->lcdClear();
      LCD->lcdPrint(0, 0, "kWh: ");  LCD->lcdPrint(0, 5, String((float)Wh / 1000.0) + " kWh");
      LCD->lcdPrint(1, 0, "Money: ");  LCD->lcdPrint(1, 7, String(totalMoney) + " $");
    }
  }
}

void saveWh() {
  EEPROM.put(0, ADE->calculateWh(voltage, current, power));
  EEPROM.put(10, totalMoney);

  boolean ok = EEPROM.commit();
  if(ok) {
    Serial.println("Save data success!");
  }
  else {
    Serial.println("Save data fail!");
  }
}

void calculateTotalMoney() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Wh = ADE->calculateWh(voltage, current, power);
  // RTC->getCurrentTime(currentHour, currentMinutes, currentSecond);
  if((currentHour >= START_TIME_1_HOUR && currentMinutes >= START_TIME_1_MINUTES && !isDone)) {
    counter++;
    if(counter < DURATION_TIME_1) {
      totalMoney += Wh * PRICE_PEAK;
      return;
    }
    else {
      isDone = true;
      counter = 0;
    }
  }

  else if((currentHour >= START_TIME_2_HOUR && currentMinutes >= START_TIME_2_MINUTES && !isDone_)) {
    counter_++;
    if(counter < DURATION_TIME_2) {
      totalMoney += Wh * PRICE_PEAK;
      return;
    }
    else {
      isDone_ = true;
      counter = 0;
    }    
  }

  if(currentHour == 0) {
    isDone = false;
    isDone_ = false;
  }
   totalMoney += Wh * PRICE_NOMARL;
}
