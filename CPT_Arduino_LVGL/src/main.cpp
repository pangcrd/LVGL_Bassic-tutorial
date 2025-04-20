#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h> 
#include "touch.h"
#include "ui.h"


static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[screenWidth*30];


TFT_eSPI my_lcd = TFT_eSPI();

// #if USE_LV_LOG != 0
// /* Serial debugging */
// void my_print(const char * buf)
// {
//     Serial.printf(buf);
//     Serial.flush();
// }
// #endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    my_lcd.setAddrWindow(area->x1, area->y1,  w, h);
    my_lcd.pushColors((uint16_t *)&color_p->full, w*h, true);
    lv_disp_flush_ready(disp);
}

/* Reading input device (simulated encoder here) */
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}
/** Screen brightness dimmed */
void TFT_SET_BL(uint8_t Value) {
  if (Value > 100) {
    printf("TFT_SET_BL Error \r\n");
    return;
  }
  uint8_t minValue = 5;  // Độ sáng tối thiểu
  if (Value < minValue) Value = minValue;

  uint8_t pwmValue = (uint8_t)(Value * 2.55);  
  analogWrite(TFT_BL, pwmValue);
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    my_lcd.init();
    my_lcd.setRotation(1);  
    touch_init(my_lcd.width(), my_lcd.height(),my_lcd.getRotation());

    TFT_SET_BL(50);

    lv_init();
   

// #if USE_LV_LOG != 0
//     lv_log_register_print_cb(my_print); /* register print function for debugging */
// #endif

    lv_disp_draw_buf_init(&disp_buf, buf, NULL, screenWidth*30);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = my_lcd.width();
    disp_drv.ver_res = my_lcd.height();
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

}

void loop()
{
    lv_task_handler(); /* let the GUI do its work */
    delay(5);
   
}
