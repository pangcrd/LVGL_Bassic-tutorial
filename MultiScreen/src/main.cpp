
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Example Multi-screen. For TFT LCD use Rotary Encoder + ESP32          // 
// Design UI on Squareline Studio                                        //
// Youtube:https://www.youtube.com/@pangcrd                              //
// Github: https://github.com/pangcrd                                    //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ui.h"
#include "EncoderRead.h"
#include "freeRTOS\freeRTOS.h"
#include "freeRTOS\task.h"


EncoderRead encoder(33, 32, 34); //PinA, PinB,buttons (PinA and PinB must be connected to interrupt-supported pins).


#define LVGL_TICK_PERIOD 20

//const int BUZZER_PIN = 25;


// Set the screen resolution
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10]; 

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

//declare the function to create a group and input device for the rotary encoder
lv_indev_t *encoder_indev = NULL;
lv_group_t *groups[3]; // Set group number

int current_screen = 0;// Default is screen 1
//static boolean setBuzzerFlag = false;

TaskHandle_t Task1;
//TaskHandle_t Task2;

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

void set_group(){

  //Create group for 3 Screen 
    groups[0] = lv_group_create(); 
    groups[1] = lv_group_create(); 
    groups[2] = lv_group_create(); 

    // Add object to group 1 screen 1
    lv_group_add_obj(groups[0], ui_Button1);
    lv_group_add_obj(groups[0], ui_Button2);
    // Add object to group 2 screen 2
    lv_group_add_obj(groups[1], ui_Button3);
    lv_group_add_obj(groups[1], ui_Button4);
    lv_group_add_obj(groups[1], ui_Button5);
    // Add object to group 3 screen 3
    lv_group_add_obj(groups[2], ui_Button6);
    lv_group_add_obj(groups[2], ui_Button7);
    lv_group_add_obj(groups[2], ui_Button8);

    // Set default group
    lv_group_set_default(groups[0]);
}

void button_state(lv_obj_t *button, bool Pressed) {
    if (Pressed) {
        lv_event_send(button, LV_EVENT_PRESSED, NULL); 
        lv_obj_clear_state(button, LV_STATE_PRESSED);  
        lv_obj_add_state(button, LV_STATE_DEFAULT);   
    }
}
// ========== Buzzer beeper START ==========
// void buzzer (void *pvParameters){
//     static uint32_t buzzerTimePrev = 0L;
//     static boolean buzzerIsBusy = false;

//     while(1){
//         if (setBuzzerFlag && !buzzerIsBusy) {
//                 buzzerTimePrev = millis();
//                 setBuzzerFlag = false; // Reset flag
//                 buzzerIsBusy = true;
//                 digitalWrite(BUZZER_PIN, HIGH);
//             }

//             if (buzzerIsBusy && millis() - buzzerTimePrev > 75) {
//                 buzzerIsBusy = false;
//                 digitalWrite(BUZZER_PIN, LOW);
//             }
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
    
// }
 // ========== Buzzer beeper END   ==========

void group_action(void *pvParameters) {

        while(1) {
            // get current group base on current screen
            lv_group_t *current_group = groups[current_screen];
            lv_indev_set_group(encoder_indev, current_group); // Add group for encoder

            // Get focus object
            lv_obj_t *focused_obj = lv_group_get_focused(current_group);

            if (encoder.encBtn()) { //Action when press encoder button
                //setBuzzerFlag = true;
                if (current_screen == 0) { // Screen 1
                    if (focused_obj == ui_Button1) {
                        lv_scr_load(ui_Screen2); // Goto screen 2
                        current_screen = 1;
                        button_state(focused_obj, true);

                } else if (focused_obj == ui_Button2) {
                    current_screen = 2; //// Goto screen 3
                    button_state(focused_obj, true);

                }
                // Screen 2 Action
                } else if (current_screen == 1) {
                    if (focused_obj == ui_Button3) {
                    lv_scr_load(ui_Screen1);
                    current_screen = 0; //Goto Screen 1 when button3 pressed
                    button_state(focused_obj, true);

                } else if (focused_obj == ui_Button4) {
                     //control function Here
                    button_state(focused_obj, true);

                } else if (focused_obj == ui_Button5) {
                    button_state(focused_obj, true);
                     //control function Here
                }
                 // Screen 3 Action
                } else if (current_screen == 2) {
                    if (focused_obj == ui_Button6) {
                        current_screen = 0; ////Goto Screen 1 when button6 pressed
                        lv_scr_load(ui_Screen1);
                        button_state(focused_obj, true);  
                } else if (focused_obj == ui_Button7) {
                        //control function Here
                        button_state(focused_obj, true);   
                } else if (focused_obj == ui_Button8) {
                         //control function Here
                        button_state(focused_obj, true);   
                }
            }
        } 
         vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);

    encoder.begin();

    lv_init();
    

    tft.begin();
    tft.setRotation(3);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    pinMode(26, OUTPUT);
    //pinMode(BUZZER_PIN, OUTPUT);

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

    set_group();

    xTaskCreatePinnedToCore(group_action,"GroupTask",8192,NULL,4,&Task1,1); //Run Group Task on core1
    //xTaskCreatePinnedToCore(buzzer,"BuzzerTask",2048,NULL,2,&Task2,0);

    Serial.println("Setup done");
}

void loop() {
    lv_timer_handler();
    delay(5);
}


