#ifndef _MY_EEPROM_H_
#define _MY_EEPROM_H_

#include "Arduino.h"

class MyEEPROM {
public:
  static MyEEPROM* getInstance() {
    static MyEEPROM obj;
    return &obj;
  }
  void initEEPROM(int size);
  void writeEEPROM(char addr, String data);
  String readEEPROM(char addr);
};

#endif