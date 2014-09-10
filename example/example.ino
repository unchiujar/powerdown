#include "Powerdown.h"

Powerdown* power = Powerdown::make(ATMEGA328, VOLTAGE_3V);

void setup() {
  Serial.begin(19200);
  Serial.println("Serial initialized");
}

void loop() {
  Serial.println("Going to sleep");
  // add a delay so the serial messages get printed out correctly 
  delay(1000);
  power->save(10000);
  Serial.println("Waked up");
}
