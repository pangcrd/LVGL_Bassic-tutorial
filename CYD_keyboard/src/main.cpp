
/**
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Example CYD  ESP32-2432S028R LVGL KEYBOARD WIDGET                       //
// Design UI on Squareline Studio. LVGL V9.1                               //
// Youtube:https://www.youtube.com/@pangcrd                                //
// Github: https://github.com/pangcrd                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
**/

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include "lvgl.h"
#include "ui.h"
#include <Arduino.h>



/** Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/
/** Change to your screen resolution **/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

TFT_eSPI tft = TFT_eSPI( screenWidth, screenHeight ); /** TFT instance **/
/** Touch screen config **/
#define XPT2046_IRQ 36 
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass tsSpi = SPIClass(VSPI);
XPT2046_Touchscreen ts (XPT2046_CS, XPT2046_IRQ);

/** Run calib_touch files to get value  **/
uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240,touchScreenMaximumY = 3800; 

/** Display flushing **/
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

/** ========== Read Touch ==========**/
void my_touch_read (lv_indev_t *indev_drv, lv_indev_data_t * data)
{
    if(ts.touched())
    {
        TS_Point p = ts.getPoint();
        /** Some very basic auto calibration so it doesn't go out of range **/
        if(p.x < touchScreenMinimumX) touchScreenMinimumX = p.x;
        if(p.x > touchScreenMaximumX) touchScreenMaximumX = p.x;
        if(p.y < touchScreenMinimumY) touchScreenMinimumY = p.y;
        if(p.y > touchScreenMaximumY) touchScreenMaximumY = p.y;
        /** Map this to the pixel position **/
        data->point.x = map(p.x,touchScreenMinimumX,touchScreenMaximumX,1,screenWidth); /** Touchscreen X calibration **/
        data->point.y = map(p.y,touchScreenMinimumY,touchScreenMaximumY,1,screenHeight); /** Touchscreen Y calibration **/
        data->state = LV_INDEV_STATE_PR;

        // Serial.print( "Touch x " );
        // Serial.print( data->point.x );
        // Serial.print( " y " );
        // Serial.println( data->point.y );
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/** Set tick routine needed for LVGL internal timings **/
static uint32_t my_tick_get_cb (void) { return millis(); }

/** Create timer for control function **/
void input_text_cb(lv_timer_t * timer) {

    if (input_text[0] != '\0') {  // Nếu có dữ liệu nhập vào
      Serial.printf("Dữ liệu nhận được: %s\n", input_text);
      input_text[0] = '\0';  // Xóa dữ liệu sau khi xử lý
  }
  
}



void setup (){

    Serial.begin( 115200 );
    lv_init();

    //Initialise the touchscreen
    tsSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); /* Start second SPI bus for touchscreen */
    ts.begin(tsSpi);      /* Touchscreen init */
    ts.setRotation(3);   /* Inverted landscape orientation to match screen */

    tft.begin();         /* TFT init */
    tft.setRotation(3); /* Landscape orientation, flipped */
                                             
    

    static lv_disp_t* disp;
    disp = lv_display_create( screenWidth, screenHeight );
    lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
    lv_display_set_flush_cb( disp, my_disp_flush );

    //Initialize the Rotary Encoder input device. For LVGL version 9+ only
    lv_indev_t *touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, my_touch_read);


    lv_tick_set_cb( my_tick_get_cb );

    ui_init();

 

    /** lv timer for run task */
    //lv_timer_create(led_update_cb, 5, NULL);

    Serial.println( "Setup done" );
}



void loop ()
{   
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}