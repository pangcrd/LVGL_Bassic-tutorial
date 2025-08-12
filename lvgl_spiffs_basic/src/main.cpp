/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Example SPIFFS LVGL. For TFT LCD use Rotary Encoder + ESP32             //
// Youtube:https://www.youtube.com/@pangcrd                                //
// Github: https://github.com/pangcrd                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include <SPIFFS.h>
#include "FS.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "EncoderRead.h"
#include <lvgl.h>

#define LED_PIN 2

bool btn_state = false;
/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/
EncoderRead encoder(33, 32, 34); //PinA, PinB,buttons (PinA and PinB must be connected to interrupt-supported pins).

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

TFT_eSPI tft = TFT_eSPI( screenWidth, screenHeight ); /* TFT instance */

//declare the function to create a group and input device for the rotary encoder
lv_obj_t * label;
lv_obj_t * btn1; 
lv_indev_t *encoder_indev = NULL;
lv_group_t *group; 

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
    
    static int32_t last_counter = 0;

    int32_t counter = encoder.getCounter();
    bool btn_state = encoder.encBtn();

    data->enc_diff = counter - last_counter;
    data->state = btn_state ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    last_counter = counter;
    // Serial.print( "Encoder counter: " );
    // Serial.println( counter );
}

/*Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb (void) { return millis(); }

/** //////////////// Simple toggle button UI //////////////////////// */
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}
void ui_init(void)
{
    lv_obj_t * text = lv_label_create(lv_screen_active());
    lv_label_set_text(text, "LVGL SPIFFS Button Example");
    lv_obj_set_style_text_color(text, lv_color_hex(0xFF16A1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(text, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(text, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_align(text, LV_ALIGN_TOP_MID, 0, 50);  


    btn1 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_size(btn1, 150, 60);  

    lv_obj_align_to(btn1, text, LV_ALIGN_OUT_BOTTOM_MID, 0, 30); 
  

    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);

  
    label = lv_label_create(btn1);
    lv_label_set_text(label, "Press Me");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(label);
}
/** ///////////////////////////////////////////////////////////// */
/** ========== Group and config input device ========== */

 void group_init(){

    //Initialize the Rotary Encoder input device. For LVGL version 9+ only
    lv_indev_t *encoder_indev = lv_indev_create();
    lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder_indev, my_encoder_read);

    // Create a group for the encoder
    group = lv_group_create();

    // Add obj into group
    lv_group_add_obj(group, btn1);

 }
 void updateButtonState(){
    if (btn_state){
        lv_obj_clear_state(btn1, LV_STATE_CHECKED);
        lv_label_set_text(label, "Toggled OFF");
        digitalWrite(LED_PIN, LOW);
        } else {
        lv_obj_add_state(btn1, LV_STATE_CHECKED);
        lv_label_set_text(label, "Toggled ON");
        digitalWrite(LED_PIN, HIGH);
        }
 }
 /** ///////////////////////////////////////////////////////////// */
 /** ========== Add SPIFFS ========== */
void spiffs_read(){
    if (!SPIFFS.begin(true)){
    return;
}
File  file = SPIFFS.open("/state.txt","r");
if (file)
{
    String saveState = file.readString();
    saveState.trim(); //remove space
    btn_state = (saveState == "1");
    file.close();
    Serial.println ("loaded state from SPIFFS: " + String (btn_state));
    updateButtonState();
}else{
    btn_state = false;
}

}
 void spiffs_write(){
    File file = SPIFFS.open("/state.txt", "w");
    if (file){
        file.print( btn_state ? "1":"0");
        file.close();
        Serial.println("State save  to SPIFFS:" + String (btn_state));
    }
}


/** ///////////////////////////////////////////////////////////// */

/** ========== Add control gui ========== */
void GUI_action(){
    lv_obj_t *focused_obj = lv_group_get_focused(group);
    
    lv_indev_set_group(encoder_indev, group);

    if (encoder.encBtn()) {
            if (focused_obj == btn1) {
             btn_state = !btn_state;
             updateButtonState();
             spiffs_write();
                 
          }
      }
}

/** ///////////////////////////////////////////////////////////// */

void setup (){
    Serial.begin( 115200 );

    encoder.begin();
    pinMode( LED_PIN, OUTPUT );

    lv_init();
    

    tft.begin();          /* TFT init */
    tft.setRotation( 1 ); /* Landscape orientation, flipped */

    static lv_disp_t* disp;
    disp = lv_display_create( screenWidth, screenHeight );
    lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
    lv_display_set_flush_cb( disp, my_disp_flush );

    lv_tick_set_cb( my_tick_get_cb );

    ui_init();
    group_init();
    spiffs_read();
    

    Serial.println( "Setup done" );
}



void loop ()
{   
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); 
    GUI_action(); /* Call the GUI action function to handle button toggling */
}
