//**********************************
//Version 1.0
//**********************************

#include "Arduino.h"
#include <SPI.h>
#include "ADE7753.h"
#include <math.h>
#define ISP_DELAY 4  //This time is needed between SPI sent bytes, at least 4μs

ADE7753::ADE7753() {
}

void ADE7753::start(uint8_t chipSelect_Pin) {
  pinMode(chipSelect_Pin, OUTPUT);
  digitalWrite(chipSelect_Pin, LOW);  // Dont controll CS pin
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV32, MSBFIRST, SPI_MODE1));
  delay(10);
}
long ADE7753::getInterruptStatus() {
  return read16bits(STATUS);
}
long ADE7753::getResetInterruptStatus() {
  return read16bits(RSTSTATUS);
}
//-----------------------------------------------------------------------------------------------
//To minimize noise synchronize the reading with the zero crossing

long ADE7753::getVRMS(int N) {
  unsigned long VRMS_VAL = 0;
  long lastupdate = 0;
  for (int i = 0; i < N; i++) {
    getResetInterruptStatus();  //clear interrupts
    lastupdate = millis();
    while (!(getInterruptStatus() & (ZX))) {
      if ((millis() - lastupdate) > 100) {
        break;
      }
    }
    VRMS_VAL += read24bits(VRMS);
  }

  return VRMS_VAL / N;
}

long ADE7753::getIRMS(int N) {
  unsigned long IRMS_VAL = 0;
  long lastupdate = 0;
  for (int i = 0; i < N; i++) {
    getResetInterruptStatus();  //clear interrupts
    lastupdate = millis();
    while (!(getInterruptStatus() & (ZX))) {
      if ((millis() - lastupdate) > 100) {
        break;
      }
    }
    IRMS_VAL += read24bits(IRMS);
  }
  return IRMS_VAL / N;
}

long ADE7753::getPERIOD(int N) {
  unsigned long PERIOD_VAL = 0;
  long lastupdate = 0;
  for (int i = 0; i < N; i++) {
    getResetInterruptStatus();  //clear interrupts
    lastupdate = millis();
    while (!(read16bits(STATUS) & (ZX))) {
      if ((millis() - lastupdate) > 100) {
        break;
      }
    }
    PERIOD_VAL += read16bits(PERIOD);
  }
  return PERIOD_VAL / N;
}

uint8_t ADE7753::getVersion() {
  return read8bits_s(DIEREV);
}

/* Convert data get from register of ADE7753 to real value */

float ADE7753::getFrequency() {
  return 447323215 / getPERIOD(5) / 1000.0;
}

float ADE7753::getVoltage() {
  write16(MODE, 0x8C);
  long temp = (getVRMS(10) * kV + offsetV) / 10000;
  voltageReal = TransfomerRatio * temp / 1000.0;
  return voltageReal;
}

float ADE7753::getCurrent() {
  write16(MODE, 0x8C);
  long temp = (getIRMS(10) * kI + offsetI) / 10000;
  currentReal = (float)(CurrentRatio * temp / 1000.0);
  return currentReal;
}

float ADE7753::getPower() {
  getVoltage();
  getCurrent();
  PPowerReal = voltageReal * currentReal * getPF();
  return PPowerReal;
}

float ADE7753::getPF() {
  long P_Reg = read24bits(LAENERGY);
  long S_Reg = read24bits(LVAENERGY);
  getResetInterruptStatus();
  PF = 0.827 * P_Reg  / S_Reg;
  if(PF > 1.0) PF = 1;
  return PF;
}

float ADE7753::calculateWh(float &v, float &i, float &p) {
  getPower();
  v = voltageReal;
  i = currentReal;
  p = PPowerReal;
  Wh = (float)(Wh * (PPowerReal * (millis() - millisKwh) / 3600000.0));
  millisKwh = millis();
  return Wh;
}

void ADE7753::calibVoltage(long kv, long offset, float ratio) {
  kV = kv;
  offsetV = offset;
  TransfomerRatio = ratio;
}

void ADE7753::calibCurrent(long ki, long offset, float ratio) {
  kI = ki;
  offsetI = offset;
  CurrentRatio = ratio;
}

void ADE7753::setWh(float _wh) {
  Wh = _wh;
}

/* SPI COMUNICATION */
unsigned char ADE7753::read8bits(char reg) {
  unsigned char b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return b0;
}

signed char ADE7753::read8bits_s(char reg) {
  unsigned char b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return b0;
}
unsigned int ADE7753::read16bits(char reg) {
  unsigned char b1, b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b1 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return (unsigned int)b1 << 8 | (unsigned int)b0;
}

signed int ADE7753::read16bits_s(char reg) {
  unsigned char b1, b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b1 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return b1 << 8 | b0;
}

unsigned long ADE7753::read24bits(char reg) {
  unsigned char b2, b1, b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b2 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b1 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return (unsigned long)b2 << 16 | (unsigned long)b1 << 8 | (unsigned long)b0;
}

signed long ADE7753::read24bits_s(char reg) {
  unsigned char b2, b1, b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b2 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b1 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  return b2 << 16 | b1 << 8 | b0;
}

signed long ADE7753::read24bits_i(char reg) {
  byte b3, b2, b1, b0;
  delayMicroseconds(ISP_DELAY);
  SPI.transfer(reg);
  delayMicroseconds(ISP_DELAY);
  b2 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b1 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  b0 = SPI.transfer(0x00);
  delayMicroseconds(ISP_DELAY);
  if (b2 > 0x29) {
    b3 = 0xFF;
  } else {
    b3 = 0x00;
  }
  return (unsigned long)b3 << 24 | (unsigned long)b2 << 16 | (unsigned long)b1 << 8 | (unsigned long)b0;
}

void ADE7753::write8(char reg, unsigned char data) {
  unsigned char data0 = 0;
  // 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
  // For Write -> DB7 = 1  / For Read -> DB7 = 0
  reg |= WRITE;
  data0 = (unsigned char)data;

  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)reg);  //register selection
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)data0);
  delayMicroseconds(ISP_DELAY);
}

void ADE7753::write16(char reg, unsigned int data) {
  //enable();
  unsigned char data0 = 0, data1 = 0;
  // 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
  // For Write -> DB7 = 1  / For Read -> DB7 = 0
  reg |= WRITE;
  //split data
  data0 = (unsigned char)data;
  data1 = (unsigned char)(data >> 8);

  //register selection, we have to send a 1 on the 8th bit to perform a write
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)reg);
  delayMicroseconds(ISP_DELAY);
  //data send, MSB first
  SPI.transfer((unsigned char)data1);
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)data0);
  delayMicroseconds(ISP_DELAY);
}

void ADE7753::write16_s(char reg, int data) {
  unsigned char data0 = 0, data1 = 0;
  // 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
  // For Write -> DB7 = 1  / For Read -> DB7 = 0
  reg |= WRITE;
  //split data
  data0 = (signed char)data;
  data1 = (signed char)(data >> 8);

  //register selection, we have to send a 1 on the 8th bit to perform a write
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)reg);
  delayMicroseconds(ISP_DELAY);
  //data send, MSB first
  SPI.transfer((signed char)data1);
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((signed char)data0);
  delayMicroseconds(ISP_DELAY);
}

void ADE7753::write24(char reg, unsigned long data) {
  unsigned char data0 = 0, data1 = 0, data2 = 0;
  reg |= WRITE;
  data0 = (unsigned char)data;
  data1 = (unsigned char)(data >> 8);
  data2 = (unsigned char)(data >> 16);
  //float = 32 bit, met de 8 MSBs van data wordt dus geen rekening
  //gehouden
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)reg);
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)data2);
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)data1);
  delayMicroseconds(ISP_DELAY);
  SPI.transfer((unsigned char)data0);
  delayMicroseconds(ISP_DELAY);
}