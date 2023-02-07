#include "LCD.h"

void LCD::initLCD(LiquidCrystal_I2C* lcd) {
  mLCD = lcd;
  mLCD->begin();
  mLCD->backlight();
  mLCD->clear();
}

void LCD::lcdClear() {
  mLCD->clear();
}
