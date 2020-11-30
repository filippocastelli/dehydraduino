#include "Arduino.h"
#include "Relay.h"

//Relay constructor
Relay::Relay(int relaySetPin){
  relayPin = relaySetPin;
}

bool Relay::setState(bool inputState){
  state = inputState;
  writeState();
}

void Relay::writeState(){
  digitalWrite(relayPin, not state);
}
