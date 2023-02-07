#ifndef _LCD_H_
#define _LCD_H_

#include "iostream"
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


class LCD {
private:
  LiquidCrystal_I2C *mLCD;
public:
  static LCD* getInstance() {
    static LCD obj;
    return &obj;
  }
  void initLCD(LiquidCrystal_I2C* lcd);
  template<typename T> 
  void lcdPrint(const uint8_t row, const uint8_t col, T data) {
    mLCD->setCursor(col, row);
    mLCD->print(data);
  }
  void lcdClear();
};

#endif  // _SERIAL_H_