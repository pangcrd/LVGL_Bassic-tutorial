// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.11
// Project name: DHT22

#ifndef _DHT22_UI_H
#define _DHT22_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"


// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Label6;
extern lv_obj_t * ui_Label7;
extern lv_obj_t * ui_Label8;
extern lv_obj_t * ui_humiIcon;
extern lv_obj_t * ui_tempIcon;
extern lv_obj_t * ui_tempVal;
extern lv_obj_t * ui_humiVal;
// CUSTOM VARIABLES

// EVENTS

extern lv_obj_t * ui____initial_actions0;

// IMAGES AND IMAGE SETS
LV_IMG_DECLARE(ui_img_humidity_png);    // assets/humidity.png
LV_IMG_DECLARE(ui_img_temperature_png);    // assets/temperature.png

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif