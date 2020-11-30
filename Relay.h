#ifndef RELAY_H
#define RELAY_H

class Relay {
  public:
    int relayPin;
    Relay(int relaySetPin);

    bool state = false;
    bool switchState();

    bool setState(bool inputState);

    void writeState();
};

#endif
