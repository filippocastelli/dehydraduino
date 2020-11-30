#include "Arduino.h"
#include "Thermostat.h"

Thermostat::Thermostat(MAX6675& thermoCouple, Timer& jobtim, Relay& rel, int pollTime) :
  thermo(thermoCouple),
  jobTimer(jobtim),
  heaterRelay(rel),
  pollTimer(pollTime) {
  currentTemp = thermo.readCelsius();
  pollTimer.start();
}

void Thermostat::start() {
  isActive = true;
  jobTimer.start();
}

void Thermostat::stop() {
  isActive = false;
  heaterRelay.setState(false);
}

float Thermostat::readTemp() {
  if (pollTimer.isOver()) {
    currentTemp = thermo.readCelsius();
    pollTimer.start();
  }
  return currentTemp;
}

void Thermostat::setMinutes(int minutes) {
  jobTimer.setMinutes(minutes);
}

void Thermostat::setSeconds(int seconds) {
  jobTimer.setSeconds(seconds);
}

String Thermostat::tempString() {
  return (String) readTemp() + char(223) + "/" + (String) (float) targetTemp + char(223) + heatingChar();
}

long Thermostat::remainingTime() {
  return jobTimer.remainingTime();
}

String Thermostat::timerString() {
  if (not jobTimer.isOver()) {
    String rem = toHMS(jobTimer.remainingSeconds());
    String tot = toHMS((int) (jobTimer.time_set / 1000));
    return (String) rem + "/" + tot;
    //return (String) jobTimer.remainingSeconds() + "s/" + (String) (int) (jobTimer.time_set / 1000) + "s";
  }
  else {
    return "FINISHED";
  }
}

String Thermostat::toHMS(int totseconds) {
  int seconds, hours, minutes;
  minutes = totseconds / 60;
  seconds = totseconds % 60;
  hours = minutes / 60;
  minutes = minutes % 60;

  return String(hours) + ":" + String(minutes) + ":" + String(seconds) + "s";
}

void Thermostat::thermoUpdate() {
  //Serial.println("thermoUpdate");
  readTemp();
  //    Serial.println("currentTemp < targetTemp");
  //    Serial.println(currentTemp < targetTemp);
  //    Serial.println("jobTimer.isOver()");
  //    Serial.println(jobTimer.isOver());
  //    Serial.println("isactive");
  //    Serial.println(isActive);
  //    Serial.println("composition");
  //    Serial.println("jobTimer.elapsedTime");
  //    Serial.println(jobTimer.elapsedSeconds());
  //    Serial.println("jobTimer.remainingSeconds");
  //    Serial.println(jobTimer.remainingSeconds());
  //    Serial.println("composition");
  //    Serial.println((currentTemp < targetTemp) and (not jobTimer.isOver()) and isActive);
  if (isActive) {
    if ((currentTemp < targetTemp) and (not jobTimer.isOver())) {
      heaterRelay.setState(true);
      isHeating = true;
    } else {
      heaterRelay.setState(false);
      isHeating = false;
    }
  }
}

char Thermostat::heatingChar() {
  if (isHeating == true) {
    return char(126);
  }
  else {
    return char(127);
  }
}
