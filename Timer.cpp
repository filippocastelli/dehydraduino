#include "Arduino.h"
#include "Timer.h"

Timer::Timer(int time_to_set){
  time_set = (long)(int) time_to_set;
}

Timer::Timer(){};

bool Timer::isOver(){
  return (millis() - start_time) > time_set;
}

void Timer::setSeconds(int seconds){
  time_set = (unsigned long) seconds*1000;
  //Serial.println(time_set);
}

void Timer::setMinutes(int minutes){
  int seconds;
  seconds = (minutes * 60);
  setSeconds(seconds);
}
void Timer::start(){
  start_time = millis();
}

//void Timer::stop(){
//}

long Timer::elapsedTime(){
  return (long) (millis() - start_time);
}

int Timer::elapsedSeconds(){
  return (unsigned int) elapsedTime() / 1000;
}

long Timer::remainingTime(){
  switch(isOver()){
    case true: 0; break;
    case false:{
      return time_set - elapsedTime();
      //Serial.println(time_set - elapsedTime());
      break;
    }
  }
}

int Timer::remainingSeconds(){
  Serial.println("im here");
  Serial.println(remainingTime());
  Serial.println((unsigned int) (remainingTime() / 1000));
  return (unsigned int) (remainingTime() / 1000);
}
