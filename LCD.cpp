#include "LCD.h"

void LCD::initLCD(LiquidCrystal_I2C* lcd) {
  mLCD = lcd;
  mLCD->begin();
  mLCD->backlight();
  mLCD->clear();
}

void LCD::lcdPrint(const uint8_t row, const uint8_t col, const char* data) {
  mLCD->setCursor(col, row);
  mLCD->printstr(data);
}
void LCD::lcdPrint(const uint8_t row, const uint8_t col, const uint8_t data) {
  mLCD->setCursor(col, row);
  mLCD->print(data);
}

void LCD::lcdClear() {
  mLCD->clear();
}
