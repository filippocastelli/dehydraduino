#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <max6675.h>
#include "Timer.h"
#include "Relay.h"

class Thermostat{
  public:
    int runOutTime;
    int updateTime;
    int targetTemp;
    int currentTemp;

    Thermostat();
    Thermostat(MAX6675& thermoCouple, Timer& jobtim, Relay& rel, int pollTime);

    void start();
    void stop();
    float readTemp();
    void setSeconds(int seconds);
    void setMinutes(int minutes);
    long remainingTime();
    String tempString();
    String timerString();
    void thermoUpdate();
  private:
    String toHMS(int totseconds);
    char heatingChar();
    MAX6675& thermo;
    Timer& jobTimer;
    Relay& heaterRelay;
    Timer pollTimer;
    bool isActive = false;
    bool isHeating;
};

#endif
