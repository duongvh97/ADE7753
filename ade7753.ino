#include "ADE7753.h"
#define ADE ADE7753::getInstance()
#define CS_PIN 16

void setup() {
  Serial.begin(9600);
  ADE->start(CS_PIN);
  delay(10);
}

void loop() {

  Serial.println("Frequency: "+String(ADE->getFrequency())+" Hz");   
  Serial.println("Voltage: "+ String(ADE->getVoltage()) + " V"); 
  Serial.print("Current: "); Serial.print(ADE->getCurrent(), 3); Serial.println(" A");
  Serial.println("Power: "+ String(ADE->getPower()) + " W"); 
  delay(1000);
}
