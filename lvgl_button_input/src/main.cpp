/** 
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Example LVGL button input CONTROL: UI object                            // 
// Design UI on Squareline Studio. LVGL V9.1                               //
// Youtube:https://www.youtube.com/@pangcrd                                //
// Github: https://github.com/pangcrd                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////*/

#include <TFT_eSPI.h>
#include <lvgl.h>
#include "Button.h"
#include <ui.h>

ButtonInit button(35, 36); /** Button input gpio */

/** declare the function to create a group and input device for the button input */
lv_indev_t *button_indev = NULL;

lv_group_t *groups[2]; 
lv_obj_t *screens[2];  

int current_screen = 0;// Default is screen 1
bool setBuzzerFlag = false;

/** Change to your screen resolution */
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

TFT_eSPI tft = TFT_eSPI( screenWidth, screenHeight ); /* TFT instance */

/** Display flushing */
void my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap)
{ 
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    if (LV_COLOR_16_SWAP) {
        size_t len = lv_area_get_size( area );
        lv_draw_sw_rgb565_swap( pixelmap, len );
    }

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( (uint16_t*) pixelmap, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/** ========== Read the button input ========== */
void my_button_read (lv_indev_t *indev_drv, lv_indev_data_t * data){

      button.update(); /** call button */

      static bool lastMoveState = false;
      static bool lastSelectState = false;

      bool movePressed = button.isMovePressed();
      bool selectPressed = button.isSelectPressed();

      if (movePressed && !lastMoveState) {
          data->btn_id = 0;
          data->state = LV_INDEV_STATE_PR;
          data->key = LV_KEY_NEXT; 
          setBuzzerFlag = true;
      } else if (selectPressed && !lastSelectState) {
          data->btn_id = 1; 
          data->state = LV_INDEV_STATE_PR; 
          data->key = LV_KEY_ENTER; 
          setBuzzerFlag = true;
      } else {
          data->state = LV_INDEV_STATE_REL;
      }

      lastMoveState = movePressed;
      lastSelectState = selectPressed;
}

/** Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb (void) { return millis(); }

/** ========== Buzzer beeper START ==========*/
void buzzer(){
    static uint32_t buzzerTimePrev = 0L;
    static boolean buzzerIsBusy = false;

    if (setBuzzerFlag && !buzzerIsBusy) {
        buzzerTimePrev = millis();
        setBuzzerFlag = false; // Reset flag
        buzzerIsBusy = true;

        tone(25, 1800); 
    }
    if (buzzerIsBusy && millis() - buzzerTimePrev > 30) {
        tone(25, 700); 
    }

    if (buzzerIsBusy && millis() - buzzerTimePrev > 60) {
        buzzerIsBusy = false;
        noTone(25); 
    }
}

void startupSound() {

    int melody[] = { 1319, 1568, 1760, 1175 };  
    int duration[] = { 200, 250, 300, 400 };   

    for (int i = 0; i < 4; i++) {
        tone(25, melody[i], duration[i]);
        delay(duration[i] + 100);
    }
    noTone(25);
}

 /** ========== Buzzer beeper END   ==========*/

/** ========== Group and config input device ==========*/
 void group_init(){

    /** Initialize the button input device. For LVGL version 9+ only */
    button_indev = lv_indev_create();
    lv_indev_set_type(button_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(button_indev, my_button_read);


    /** Number of screen and group. I use 2 group for 2 screen.*/
    screens[0] = ui_Screen1;
    screens[1] = ui_Screen2;

    for (int i = 0; i < 2; i++) {
        groups[i] = lv_group_create(); 
    }

    /** Add obj into group */ 
    lv_group_add_obj(groups[0], ui_Button1);
    lv_group_add_obj(groups[0], ui_Button2);
    lv_group_add_obj(groups[0], ui_Button3);
    lv_group_add_obj(groups[1], ui_Button4);
    lv_group_add_obj(groups[1], ui_Button5);
    
    /** Set group for button. */ 
    lv_indev_set_group(button_indev, groups[0]);
    lv_scr_load(screens[0]);

}

void change_screen(int new_screen) {

  if (new_screen == current_screen) return;  /** No need to reload even on the same screen */

  current_screen = new_screen;

  lv_scr_load(screens[current_screen]);  

  /** Update the first group and focus object */ 
  lv_indev_set_group(button_indev, groups[current_screen]);
  
}

void gui_action (){

  static bool lastSelectState = false;
  bool selectPressed = button.isSelectPressed();

  if (selectPressed && !lastSelectState) {
    
    lv_obj_t *focused_obj = lv_group_get_focused(groups[current_screen]); /** Get focus obj on group */

    switch (current_screen) {
      case 0:
          if (focused_obj == ui_Button1); /** Add control function here */
          else if (focused_obj == ui_Button2);
          else if (focused_obj == ui_Button3) change_screen(1);
          break;

      case 1:
          if (focused_obj == ui_Button4);/** Add control function here */
          else if (focused_obj == ui_Button5) change_screen(0);
          break;
  }
}
  lastSelectState = selectPressed;
}

void setup() {
    Serial.begin(115200);
    button.begin(); 

    startupSound();

    pinMode(26,OUTPUT); /** LED */
    pinMode(25,OUTPUT); /** Buzzer */

    lv_init();

    tft.begin();          /** TFT init */
    tft.setRotation(0); /** Landscape orientation, flipped */

    static lv_disp_t* disp;
    disp = lv_display_create( screenWidth, screenHeight );
    lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
    lv_display_set_flush_cb( disp, my_disp_flush );

    lv_tick_set_cb( my_tick_get_cb );

    ui_init();
    group_init();

    Serial.println( "Setup done" );
}

void loop() {   
    lv_timer_handler(); /** let the GUI do its work **/
    buzzer();
    gui_action();
    delay(5);
}
