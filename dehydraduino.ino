#include <Arduino.h>
#include <max6675.h>

#include "header.h"

void setup() {
  pinMode(encBtn,INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Arduino Menu Library");Serial.flush();
  encoder.begin();
  lcd.begin(16,2);
  lcd.setBacklight(255);
  nav.idleTask=idle;//point a function to be used when menu is suspended
  nav.showTitle=false;
  relay.writeState();
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  relay.writeState();
  thermostat.thermoUpdate();
  //delay(100);//simulate a delay as if other tasks are running
}
