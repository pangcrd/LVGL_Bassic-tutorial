// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.2
// LVGL version: 9.1.0
// Project name: buttoninput

#include "ui.h"
#include <Arduino.h>

void ledON(lv_event_t * e)
{
	digitalWrite(26,HIGH);
}

void ledOFF(lv_event_t * e)
{
	digitalWrite(26,LOW);
}
