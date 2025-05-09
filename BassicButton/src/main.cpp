#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ui.h"
#include "EncoderRead.h"


EncoderRead encoder(33, 32, 34); //PinA, PinB,buttons (PinA and PinB must be connected to interrupt-supported pins).


#define LVGL_TICK_PERIOD 20

// Set the screen resolution
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10]; 

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

//declare the function to create a group and input device for the rotary encoder
lv_group_t *group1 = NULL;
lv_indev_t *encoder_indev = NULL;

//configure the resolution
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}
//Read the counting value of the encoder and handle the button press.
//Docs: https://docs.lvgl.io/master/details/main-components/indev.html

void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {

    static int32_t last_counter = 0;

    int32_t counter = encoder.getCounter();
    bool btn_state = encoder.encBtn();

    data->enc_diff = counter - last_counter;
    data->state = btn_state ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    last_counter = counter;
}


void setup() {
    Serial.begin(115200);

    encoder.begin();

    lv_init();
    

    tft.begin();
    tft.setRotation(3);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    pinMode(26, OUTPUT);

    //Initialize the display.
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    //Initialize the Rotary Encoder input device.
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    encoder_indev = lv_indev_drv_register(&indev_drv);

    ui_init();

    /*Declare the UI that has been designed on Squareline and needs to be used here. */
    //Create a group.
    group1 = lv_group_create();

    lv_group_set_default(group1);//Set default group

    lv_group_add_obj(group1, ui_Button1);//Your can rename button in Squareline.If you don't remember. You can check file ui_Screen.c
    lv_group_add_obj(group1, ui_Button2);

    //lv_group_focus_obj(ui_Button1);

    
    Serial.println("Setup done");
}

void loop() {
    lv_timer_handler();
    delay(5);

    lv_obj_t *focused_obj = lv_group_get_focused(group1);
    lv_indev_set_group(encoder_indev, group1);//Set group for encoder so that the encoder can control the objects in group

    if (encoder.encBtn()) {//Handle the objects on the screen when the rotary encoder button is pressed.
            if(focused_obj == ui_Button1) {
                lv_event_send(focused_obj, LV_EVENT_PRESSED, NULL);//Send the button press event for processing.
                lv_obj_clear_state(focused_obj, LV_STATE_PRESSED); //Set effect when click button
                lv_obj_add_state(focused_obj, LV_STATE_DEFAULT); 
                Serial.println("Button 1 pressed on Screen 2");
            } else if (focused_obj == ui_Button2) {
                lv_event_send(focused_obj, LV_EVENT_PRESSED, NULL);
                lv_obj_clear_state(focused_obj, LV_STATE_PRESSED);
                lv_obj_add_state(focused_obj, LV_STATE_DEFAULT);
                Serial.println("Button 2 pressed on Screen 2"); 
            }
        }
    
}



