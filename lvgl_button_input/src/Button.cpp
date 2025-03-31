#include "Button.h"

/** Add more buttons here */

ButtonInit::ButtonInit(uint8_t pinA, uint8_t pinB)
    : _pinA(pinA), _pinB(pinB),
      moveState(true), selectState(true),
      lastMoveState(0), lastSelectState(0),
      lastDebounceTimeA(0), lastDebounceTimeB(0) {}

void ButtonInit::begin() {
    pinMode(_pinA, INPUT); 
    pinMode(_pinB, INPUT);  
}

/** Debounce Button */
bool ButtonInit::debounce(uint8_t pin, bool &lastState, unsigned long &lastDebounceTime) {
    bool currentState = digitalRead(pin);  
    
    /** Check status change*/ 
    if (currentState != lastState) {
        lastDebounceTime = millis(); 
    }
    
    /** Check debounce time */ 
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (currentState != lastState) {
            lastState = currentState;
        }
    }
    /** pull-up, press button is LOW signal*/
    return (currentState == LOW);
}

/** Update state */
void ButtonInit::update() {
    
    bool newMoveState = debounce(_pinA, lastMoveState, lastDebounceTimeA);
    bool newSelectState = debounce(_pinB, lastSelectState, lastDebounceTimeB);

    
    if (newMoveState != moveState && newMoveState) {
       //printf("Move Button Pressed\n");
    }
    if (newSelectState != selectState && newSelectState) {
       //printf("Select Button Pressed\n");
    }
    moveState = newMoveState;
    selectState = newSelectState;
}
bool ButtonInit::isMovePressed() {
    return moveState;
}

bool ButtonInit::isSelectPressed() {
    return selectState;
}