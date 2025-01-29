/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Example Animation Loading. For TFT LCD + ESP32                          // 
// Design UI on Squareline Studio. LVGL V9.1                               //
// Youtube:https://www.youtube.com/@pangcrd                                //
// Github: https://github.com/pangcrd                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include <TFT_eSPI.h>
#include "lvgl.h"
#include "ui.h"

static int value = 0;

static lv_timer_t * Slidetimer = NULL;



/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/


/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

TFT_eSPI tft = TFT_eSPI( screenWidth, screenHeight ); /* TFT instance */



/* Display flushing */
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

/*========== Read the encoder rotary ==========*/
void my_encoder_read (lv_indev_t *indev_drv, lv_indev_data_t * data)
{
    
    // static int32_t last_counter = 0;

    // int32_t counter = encoder.getCounter();
    // bool btn_state = encoder.encBtn();

    // data->enc_diff = counter - last_counter;
    // data->state = btn_state ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    // last_counter = counter;
}

/*Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb (void) { return millis(); }

// Hàm callback cho timer
static void set_value_task(lv_timer_t * t) {

    lv_bar_set_range(ui_Bar1, 0, 100);
    value++;
    lv_bar_set_value(ui_Bar1, value, LV_ANIM_ON); 

    // Dừng timer khi đạt 100
    if(value >= 100) {
        lv_timer_del(Slidetimer);
        Slidetimer = NULL;
        _ui_screen_change(&ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Screen2_screen_init);

    }
}

 
void setup ()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    lv_init();


    tft.begin();          /* TFT init */
    tft.setRotation( 1 ); /* Landscape orientation, flipped */

    static lv_disp_t* disp;
    disp = lv_display_create( screenWidth, screenHeight );
    lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
    lv_display_set_flush_cb( disp, my_disp_flush );

    //Initialize the Rotary Encoder input device. For LVGL version 9+ only
    lv_indev_t *encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, my_encoder_read);


    lv_tick_set_cb( my_tick_get_cb );

    ui_init();

    arisu_Animation(ui_arisu, 0);
    
    Slidetimer = lv_timer_create(set_value_task, 50, NULL);

    Serial.println( "Setup done" );
}



void loop ()
{   
    
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
    
    
}
