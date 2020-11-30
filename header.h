#include "Thermostat.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <menu.h>//menu macros and objects
#include <menuIO/PCF8574Out.h>//arduino I2C LCD
#include <menuIO/encoderIn.h>//quadrature encoder driver and fake stream
#include <menuIO/keyIn.h>//keyboard driver and fake stream (for the encoder button)
#include <menuIO/chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuIO/serialIn.h>
using namespace Menu;

// ---- PHYSICAL DEVICES -----
// RELAY
#define relayPin 7
Relay relay(relayPin);

// OUTPUT_LED
#define LEDPIN LED_BUILTIN

// ENCODER
#define encA 3
#define encB 4
#define encBtn 2
#define ENC_SENSIVITY 4

// THERMOCOUPLE
#define thermoCouple_CLK 10
#define thermoCouple_CS 9
#define thermoCouple_DO 8

MAX6675 thermoCouple(thermoCouple_CLK, thermoCouple_CS, thermoCouple_DO);

// LCD
LiquidCrystal_PCF8574 lcd(0x27);


// ---- OBJECTS -----
// OTHER OBJECTST
Timer timer;
Timer screenUpdateTimer(500);

Thermostat thermostat(thermoCouple, timer, relay, 500);

// MENU OBJECTS
#define MAX_DEPTH 2

encoderIn<encA, encB> encoder; //simple quad encoder driver
encoderInStream<encA, encB> encStream(encoder, ENC_SENSIVITY); // simple quad encoder fake Stream
//a keyboard with only one key as the encoder button
keyMap encBtn_map[] = {{ -encBtn, defaultNavCodes[enterCmd].ch}}; //negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys
menuIn* inputsList[] = {&encStream, &encButton};
chainStream<2> in(inputsList);

// ---- MENU DEFINITION -----
int ledCtrl = HIGH;
//int timerSeconds = 0;
int timerMinutes = 0;
int backlightstate = 255;

result myLedOn() {
  ledCtrl = HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl = LOW;
  return proceed;
}

result RelayOn() {
  relay.setState(true);
  return proceed;
}

result RelayOff() {
  relay.setState(false);
  return proceed;
}

result setbacklight() {
  lcd.setBacklight(backlightstate);
  return proceed;
}

enum MATERIAL {
  PETG,
  PLA,
  APPLES,
  NONE,
};

MATERIAL chosenMaterial = NONE;

result setChosenMaterial() {
  switch (chosenMaterial) {
    case (PLA): {
        thermostat.targetTemp = 65;
        timerMinutes = 240;
        break;
      }
    case (PETG): {
        thermostat.targetTemp = 70;
        timerMinutes = 240;
        break;
      }
    case (APPLES): {
        thermostat.targetTemp = 60;
        timerMinutes = 480;
        break;
      }

  }
}

result startThermo(eventMask e, prompt &item);


TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       , VALUE("[On]", HIGH, doNothing, noEvent)
       , VALUE("[Off]", LOW, doNothing, noEvent)
      );
TOGGLE(backlightstate, setBL, "BL: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       , VALUE("[On]", 255, setbacklight, noEvent)
       , VALUE("[Off]", 0, setbacklight, noEvent)
      );
TOGGLE(relay.state, setRelay, "Relay: ", doNothing, noEvent, noStyle
       , VALUE("[On]", true, RelayOn, noEvent)
       , VALUE("[Off]", false, RelayOff, noEvent)
      );

TOGGLE(chosenMaterial, setMaterial, "Mat:", doNothing, noEvent, noStyle
       , VALUE("[PETG]", PETG, doNothing, noEvent)
       , VALUE("[PLA]", PLA, doNothing, noEvent)
       , VALUE("[APPLES]", APPLES, doNothing, noEvent)
       , VALUE("[NONE]", NONE, doNothing, noEvent)
      );

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
     , FIELD(thermostat.targetTemp, "Temp:", "c", 0, 100, 10, 1, doNothing, noEvent, noStyle)
     , FIELD(timerMinutes, "Time:", "m", 0, 1009, 10, 1, doNothing, noEvent, noStyle)
     , SUBMENU(setLed)
     , SUBMENU(setRelay)
     , SUBMENU(setBL)
     , SUBMENU(setMaterial)
     //, OP("LED Off", myLedOff, enterEvent)
     //, OP("LED On", myLedOn, enterEvent)
     , OP("START", startThermo, enterEvent)
     , EXIT("<Back")
    );

MENU_OUTPUTS(out, MAX_DEPTH
             , LCD_OUT(lcd, {0, 0, 16, 2})
             , NONE
            );

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out); //the navigation root object

result idleThermo(menuOut& o, idleEvent e) {
  switch (e) {
    case idleStart: {
        screenUpdateTimer.start();
        setChosenMaterial();
        //thermostat.setSeconds(timerSeconds);
        thermostat.setMinutes(timerMinutes);
        thermostat.start();
        nav.idleChanged = true;
        break;
      }
    case idling: {
        nav.idleChanged = true;
        if (screenUpdateTimer.isOver()) {
          o.setCursor(0, 0);
          o.clear();
          o.print(thermostat.timerString());
          o.setCursor(0, 1);
          o.print(thermostat.tempString());
          screenUpdateTimer.start();
        }
        break;
      }
    case idleEnd: {
        thermostat.stop();
      }
  }
}
result startThermo(eventMask e, prompt &item) {
  nav.idleOn(idleThermo);
  return proceed;
}

result idle(menuOut& o, idleEvent e) {
  switch (e) {
    case idleStart: {
        o.print("suspending menu!");
        nav.idleChanged;
        break;
      };
    case idling: {
        nav.idleChanged = true;
        switch (screenUpdateTimer.isOver()) {
          case true: {
              o.clear();
              screenUpdateTimer.start();
              o.print(thermostat.tempString());
              break;
            };
          case false: {
              break;
            }
        }
        break;
      };
    case idleEnd: {
        relay.state = false;
        thermostat.stop();
        Serial.println("ending");
        break;
      }
  }
  return proceed;
}
