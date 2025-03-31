#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
 /** Tạo class cho button */
class ButtonInit {
    private:
        uint8_t _pinA, _pinB;
        bool moveState, selectState;
        bool lastMoveState, lastSelectState;
        unsigned long lastDebounceTimeA, lastDebounceTimeB;
        static const unsigned long debounceDelay = 50;

        bool debounce(uint8_t pin, bool &lastState, unsigned long &lastDebounceTime);

    public:
        ButtonInit(uint8_t pinA, uint8_t pinB);  // Constructor public
        void begin();
        void update();
        bool isMovePressed();
        bool isSelectPressed();
};

#endif
